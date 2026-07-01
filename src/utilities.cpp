
#include "utilities.h"

Settings::Settings(int argc, char** argv)
{
    PREPARE_JOB = false;
    SUBMIT_JOB = false;
    RUN_JOB = false;
    start_directory = fs::current_path();
    solvated_prmtop="";
    complex_prmtop="";
    receptor_prmtop="";
    ligand_prmtop="";
    mmpbsa_input="";
    trajectory="";
    complex_mask=":1-100";
    receptor_mask=":1-99";
    ligand_mask=":100";
    salt_concentration="0.0";

    for (int i=0; i < argc; i++)
    {
        if ((std::string)argv[i] == "--prepare")
        {
            PREPARE_JOB = true;
            continue;
        }
        if ((std::string)argv[i] == "--submit")
        {
            SUBMIT_JOB = true;
            continue;
        }
        if ((std::string)argv[i] == "--run")
        {
            RUN_JOB = true;
            continue;
        }
        if ((std::string)argv[i] == "-s")
        {
            solvated_prmtop = fs::absolute(argv[i+1]);
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-c")
        {
            complex_prmtop = fs::absolute(argv[i+1]);
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-r")
        {
            receptor_prmtop = fs::absolute(argv[i+1]);
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-l")
        {
            ligand_prmtop = fs::absolute(argv[i+1]);
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-m")
        {
            mmpbsa_input = fs::absolute(argv[i+1]);
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-t")
        {
            trajectory = fs::absolute(argv[i+1]);
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-cm")
        {
            complex_mask = argv[i+1];
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-rm")
        {
            receptor_mask = argv[i+1];
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-lm")
        {
            ligand_mask = argv[i+1];
            i++;
            continue;
        }
        if ((std::string)argv[i] == "-sc")
        {
            salt_concentration = argv[i+1];
            i++;
            continue;
        }
    }
}

Settings::~Settings()
{    
}

void silent_shell(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
}

std::string GetSysResponse(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

bool CheckProgAvailable(const char* program)
{
        std::string result;
        std::string cmd;
        cmd = "which ";
        cmd += program;
        result=GetSysResponse(cmd.c_str());
        if (result.empty())
        {
            std::cout << "Missing program: " << program << std::endl;
            return false;
        }
        return true;
}

void write_to_file(std::string inputfilename, std::string buffer)
{
    std::ofstream outFile;
    outFile.open(inputfilename,std::ios::out);
    outFile << buffer;
    outFile.close();
}

void append_to_file(std::string inputfilename, std::string buffer)
{
    if (!fs::exists(inputfilename))
    {
        write_to_file(inputfilename,"");
    }
    std::ofstream outFile;
    outFile.open(inputfilename,std::ios::app);
    outFile << buffer;
    outFile.close();
}

std::string GetTimeAndDate()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream buffer;
    buffer.str("");
    buffer << std::put_time(&tm, "%Y.%m.%d %H:%M:%S");
    return buffer.str();
}

int is_empty(const char *s) 
{
    while (*s != '\0') 
    {
        if (!isspace((unsigned char)*s))
        return 0;
        s++;
    }
    return 1;
}

std::string string_between(std::string incoming, std::string first_delim, std::string second_delim)
{
    unsigned first = incoming.find(first_delim);
    if (first == std::string::npos)
    {
        return incoming;
    }
    unsigned last = incoming.find(second_delim);
    if (last == std::string::npos)
    {
        return incoming.substr(first + 1, incoming.size());
    }
    return incoming.substr(first + 1, last - first - 1);
}

void compress_and_delete(std::string directory)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "tar -czvf " << directory << ".tar.gz "<< directory << "/ && rm -r " << directory << "/";
    silent_shell(buffer.str().c_str());
}

std::vector<std::string> sort_files_by_timestamp(std::string directory,std::string pattern)
{
    std::set <fs::path> sort_by_name;
    for (fs::path p : fs::directory_iterator(directory))
    {
        if (p.extension() == pattern) 
        {
            sort_by_name.insert(p);
        }    
    }
    std::vector<std::string> file_list={};

    for (auto p : sort_by_name)
    {
        std::cout << p << std::endl;
        file_list.push_back(p);
    }
    
    return file_list;
}
