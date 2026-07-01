#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <experimental/filesystem>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <iomanip>
#include <ctime>
#include <set>
#include <unistd.h>
#include <omp.h>

namespace fs = std::experimental::filesystem;

class Settings
{
    public:
        Settings(int argc, char** argv);
        ~Settings();
        bool PREPARE_JOB;
        bool SUBMIT_JOB;
        bool RUN_JOB;
        std::string start_directory;
        std::string solvated_prmtop;
        std::string complex_prmtop;
        std::string receptor_prmtop;
        std::string ligand_prmtop;
        std::string mmpbsa_input;
        std::string trajectory;
        std::string complex_mask;
        std::string receptor_mask;
        std::string ligand_mask;
        std::string salt_concentration;
    private:

};
void prepare_mmpbsa_job(Settings settings);
void submit_mmpbsa_job(Settings settings);
void run_mmpbsa_job(Settings settings);

void silent_shell(const char* cmd);
std::string GetSysResponse(const char* cmd);
bool CheckProgAvailable(const char* program);
void write_to_file(std::string inputfilename, std::string buffer);
void append_to_file(std::string inputfilename, std::string buffer);
std::string GetTimeAndDate();
int is_empty(const char *s);
std::string string_between(std::string incoming, std::string first_delim, std::string second_delim);
void compress_and_delete(std::string directory);
std::vector<std::string> sort_files_by_timestamp(std::string directory,std::string pattern);


#endif