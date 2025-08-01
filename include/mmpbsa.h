#ifndef MMPBSA_H
#define MMPBSA_H

#include "utilities.h"


class FileList
{
    public:
        std::string mmpbsa_input;
        std::string solvated_prmtop;
        std::string complex_prmtop;
        std::string ligand_prmtop;
        std::string receptor_prmtop;
        std::string trajectory;
        FileList(int argc, char** argv);
        ~FileList();
        void copy_to_tmp();
};

int get_traj_nframes(std::string trajfile,std::string prmtop);
std::string GetPostBuffer(std::string input_file);
std::string GetPreBuffer(std::string input_file);
bool ValidateFile(std::string filename);
#endif