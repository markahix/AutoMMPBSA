#include "utilities.h"
#include "mmpbsa.h"

int main(int argc, char** argv)
{
    FileList my_files(argc,argv);
    std::string starting_directory_location = fs::current_path();
    
    // Get original MMPBSA input file and parse it into individual chunks.
    std::string prebuffer=GetPreBuffer(my_files.mmpbsa_input);
    std::string postbuffer=GetPostBuffer(my_files.mmpbsa_input);
    
    // move necessary files into /tmp, then go there.
    my_files.copy_to_tmp();
    fs::current_path("/tmp/");

    // get number of frames.
    int n_frames = get_traj_nframes("/tmp/trajectory.mdcrd","/tmp/solvated.prmtop");

    // Create (if necessary) thread_specific directories.
    #pragma omp parallel
    {
      int my_thread_num = omp_get_thread_num();
      std::stringstream buffer;
      buffer.str("");
      buffer << "/tmp/thread_" << std::setw(2) << std::setfill('0') << my_thread_num << "/";
      std::string thread_dir = buffer.str();
      if (!fs::exists(thread_dir))
      {
          fs::create_directory(thread_dir);
      }        
    }

    //Create Final Results Folder
    fs::create_directory("/tmp/FinalResults/");
  
    // Run MMPBSA on each individual frame.
    #pragma omp parallel for
    for (int i=0; i < n_frames; i++)
    {   
        int my_thread_num = omp_get_thread_num();
        std::stringstream buffer;
        buffer.str("");
        buffer << "/tmp/thread_" << std::setw(2) << std::setfill('0') << my_thread_num << "/";
        std::string thread_dir = buffer.str();
        fs::current_path(thread_dir);

        // Create frame-specific MMPBSA input file from original mmpbsa input file.
        buffer.str("");
        buffer << thread_dir << "mmpbsa." << std::setw(6) << std::setfill('0') << i+1 << ".in";
        std::string filename = buffer.str();
        std::ofstream ofile(filename,std::ios::out);
        ofile << prebuffer;
        ofile << "startframe = " << i + 1 << std::endl;
        ofile << "endframe   = " << i + 1  << std::endl;
        ofile << "interval   = 1" << std::endl;
        ofile << postbuffer;
        ofile.close();

        // Create frame-specific output filename.
        buffer.str("");
        buffer << "/tmp/FinalResults/FINAL_RESULTS_MMPBSA." << std::setw(6) << std::setfill('0') << i+1 << ".dat";
        std::string outfilename = buffer.str();

        // run MMPBSA.py with the frame-specific input/output filenames.
        buffer.str("");
        buffer << "MMPBSA.py -O -i " << filename << " -o " << outfilename << " -sp /tmp/solvated.prmtop -cp /tmp/complex.prmtop -rp /tmp/receptor.prmtop -lp /tmp/ligand.prmtop -y /tmp/trajectory.mdcrd";
        utils::silent_shell(buffer.str().c_str());
        fs::current_path("/tmp/"); 
    }

    // Aggregate results from each output file to single csv file.
    std::string tmp = (std::string)fs::absolute(my_files.trajectory).stem();
    std::string csv_file = tmp + ".mmpbsa.dat";
    std::cout << "Aggregating results to " << csv_file << std::endl;
    std::ofstream ofile(csv_file,std::ios::out);
    ofile << "ProdStep, Delta_Avg, Delta_StDev" << std::endl;
    double prodstep = stod(tmp.substr(my_files.trajectory.size()-4,4));
    for (int i = 0; i < n_frames; i++)
    {
        std::stringstream buffer;
        buffer.str("");
        buffer << "/tmp/FinalResults/FINAL_RESULTS_MMPBSA." << std::setw(6) << std::setfill('0') << i+1 << ".dat";
        std::string outfilename = buffer.str();

        std::ifstream ifile(outfilename,std::ios::in);
        std::string line;
        while (getline(ifile,line))
        {
          if (line.find("DELTA TOTAL") == std::string::npos)
          {
            continue;
          }
          line = line.substr(11,line.size()-11);
          double delta_avg, delta_std;
          buffer.str(line);
          buffer >> delta_avg >> delta_std;
          ofile << (int)prodstep - 1 << i/n_frames << ", " << delta_avg << ", " << delta_std << std::endl;
          break;
        }
        ifile.close();
    }
    ofile.close();
    return 0;
}