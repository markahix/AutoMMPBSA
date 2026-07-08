#include "utilities.h"

void parse_mmpbsa_to_csv(std::string mmpbsa_data_file, std::string csv_file)
{
    std::ofstream ofile;
    std::ifstream infile;
    std::string line;
    std::stringstream buffer;
    if (!fs::exists(csv_file))
    {
        ofile.open(csv_file,std::ios::out);
        ofile << "ProdStep, Delta_Avg, Delta_StDev" << std::endl;
    }
    else
    {
        ofile.open(csv_file,std::ios::app);
    }

    if (! ofile.is_open())
    {
        std::cout << "ERROR:  Unable to open " << csv_file << " for writing.";
        return;
    }

    infile.open(mmpbsa_data_file,std::ios::in);
    if (! infile.is_open())
    {
        std::cout << "ERROR:  Unable to open " << mmpbsa_data_file << " for reading.";
        return;
    }
    std::string tmp = fs::path(mmpbsa_data_file).stem();
    double prodstep = stod(tmp.substr(mmpbsa_data_file.size()-4,4));
    double delta_avg, delta_std;
    while (getline(infile,line))
    {
        if (line.find("DELTA TOTAL") == std::string::npos)
        {
            continue;
        }
        line = line.substr(11,line.size()-11);
        buffer.str(line);
        buffer >> delta_avg >> delta_std;
        ofile << prodstep << ", " << delta_avg << ", " << delta_std << std::endl;
        break;
    }
    infile.close();
    ofile.close();
}

void run_mmpbsa_job(Settings settings)
{
        // Update SLURM jobname
        std::stringstream buffer;
        buffer.str("");
        buffer << "MMPBSA_for_" << settings.trajectory;

        // copy necessary files into /tmp/
        fs::copy(settings.solvated_prmtop, "/tmp/solvated.prmtop");
        fs::copy(settings.trajectory, "/tmp/trajectory.mdcrd");
        fs::copy(settings.mmpbsa_input, "/tmp/mmpbsa.in");
        fs::copy(settings.complex_prmtop, "/tmp/complex.prmtop");
        fs::copy(settings.receptor_prmtop, "/tmp/receptor.prmtop");
        fs::copy(settings.ligand_prmtop, "/tmp/ligand.prmtop");

        // move to /tmp/
        fs::current_path("/tmp/");

        // run mmpbsa.py
        buffer.str("");
        // buffer << "module load " << slurm.SLURM_amber_module << "; ";
        buffer << "MMPBSA.py -O -i mmpbsa.in -o FINAL_RESULTS_MMPBSA.dat -sp solvated.prmtop -cp complex.prmtop -rp receptor.prmtop -lp ligand.prmtop -y trajectory.mdcrd";
        silent_shell(buffer.str().c_str());

        // copy results back to appropriate filenames
        std::string mmpbsa_result = fs::path(settings.trajectory).parent_path() / fs::path(settings.trajectory).stem();
        // double prodstep = stod(mmpbsa_result.substr(mmpbsa_result.size()-4,4));
        mmpbsa_result += ".mmpbsa.dat";
        fs::copy("FINAL_RESULTS_MMPBSA.dat",mmpbsa_result);

        // append results to CSV
        std::string csv_file = std::getenv("SLURM_SUBMIT_DIR");
        csv_file += "/MMPBSA_data.csv";
        parse_mmpbsa_to_csv(mmpbsa_result, csv_file);

        // clean up
        buffer.str("");
        buffer << "rm " << std::getenv("SLURM_SUBMIT_DIR") << "/MMPBSA_*" << std::getenv("SLURM_JOB_ID") << ".out " << std::getenv("SLURM_SUBMIT_DIR") << "/MMPBSA_*" << std::getenv("SLURM_JOB_ID") << ".err";
        silent_shell(buffer.str().c_str());
        return;

}