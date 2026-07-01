#include "utilities.h"

void prepare_mmpbsa_job(Settings settings)
{
    // make MMPBSA_Inputs folder
    fs::create_directory("MMPBSA_Inputs/");

    // Generate complex.prmtop
    std::stringstream buffer;
    buffer.str("");
    buffer << "parm " << settings.solvated_prmtop << std::endl;
    buffer << "parmstrip !" << settings.complex_mask << std::endl;
    buffer << "parmwrite out MMPBSA_Inputs/complex.prmtop" << std::endl << "quit" << std::endl;
    write_to_file("parmed.in",buffer.str());
    buffer.str("");
    buffer << "cpptraj -i parmed.in > complex_parmed.log && rm parmed.in";
    silent_shell(buffer.str().c_str());

    // Generate receptor.prmtop
    buffer.str("");
    buffer << "parm " << settings.solvated_prmtop << std::endl;
    buffer << "parmstrip !" << settings.receptor_mask << std::endl;
    buffer << "parmwrite out MMPBSA_Inputs/receptor.prmtop" << std::endl << "quit" << std::endl;
    write_to_file("parmed.in",buffer.str());
    buffer.str("");
    buffer << "cpptraj -i parmed.in > receptor_parmed.log && rm parmed.in";
    silent_shell(buffer.str().c_str());

    // Generate ligand.prmtop
    buffer.str("");
    buffer << "parm " << settings.solvated_prmtop << std::endl;
    buffer << "parmstrip !" << settings.ligand_mask << std::endl;
    buffer << "parmwrite out MMPBSA_Inputs/ligand.prmtop" << std::endl << "quit" << std::endl;
    write_to_file("parmed.in",buffer.str());
    buffer.str("");
    buffer << "cpptraj -i parmed.in > ligand_parmed.log && rm parmed.in";
    silent_shell(buffer.str().c_str());

    // Generate mmpbsa.in
    buffer.str("");
    buffer << "Input file for running PB and GB" << std::endl;
    buffer << "&general" << std::endl;
    buffer << "   keep_files=1," << std::endl;
    buffer << "   entropy=0," << std::endl;
    buffer << "   ligand_mask=" << settings.ligand_mask << "," << std::endl;
    buffer << "   strip_mask=!" << settings.complex_mask << "," << std::endl;
    buffer << "   receptor_mask=" << settings.receptor_mask << std::endl;
    buffer << "/" << std::endl;
    buffer << "&gb" << std::endl;
    buffer << "  igb=2, saltcon=" << settings.salt_concentration << "," << std::endl;
    buffer << "/" << std::endl;
    buffer << "&decomp" << std::endl;
    buffer << " idecomp=1," << std::endl;
    buffer << " dec_verbose=1," << std::endl;
    buffer << "/" << std::endl;
    buffer << "" << std::endl;
    write_to_file("MMPBSA_Inputs/mmpbsa.in",buffer.str());

    // check that all files exist
    if (!fs::exists("MMPBSA_Inputs/complex.prmtop") || !fs::exists("MMPBSA_Inputs/receptor.prmtop") || !fs::exists("MMPBSA_Inputs/ligand.prmtop"))
    {
        std::cerr << "ERROR:  failed to generate MMPBSA inputs." << std::endl;
        exit(0);
    }
    int n_com_atoms = stoi(GetSysResponse("grep -A2 \"FLAG POINTERS\" MMPBSA_Inputs/complex.prmtop | tail -n 1 | awk '{print $1}'"));
    int n_rec_atoms = stoi(GetSysResponse("grep -A2 \"FLAG POINTERS\" MMPBSA_Inputs/receptor.prmtop | tail -n 1 | awk '{print $1}'"));
    int n_lig_atoms = stoi(GetSysResponse("grep -A2 \"FLAG POINTERS\" MMPBSA_Inputs/ligand.prmtop | tail -n 1 | awk '{print $1}'"));
    if (n_com_atoms != n_rec_atoms + n_lig_atoms)
    {
        std::cerr << "Error:  Ligand + Receptor does not equal Complex!" << std::endl;
        exit(0);
    }
    silent_shell("rm *parmed.log");
}
