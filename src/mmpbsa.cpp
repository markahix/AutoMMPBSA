#include "mmpbsa.h"

bool ValidateFile(std::string filename)
{
  std::ifstream ifile(filename,std::ios::in);
  if (! ifile.is_open())
  {
    std::cout << "Unable to open " << filename << std::endl;
    return false;
  }
  ifile.close();
  return true;
}

std::string GetPreBuffer(std::string input_file)
{
  std::stringstream buffer;
  std::string line;
  std::ifstream ifile(input_file,std::ios::in);
  buffer.str("");
  while (getline(ifile,line))
  {
    buffer << line << std::endl;
    if (line.find("&general") != std::string::npos)
    {
      break;
    }
  }
  ifile.close();
  return buffer.str();
}

std::string GetPostBuffer(std::string input_file)
{
  std::stringstream buffer;
  std::string line;
  std::ifstream ifile(input_file,std::ios::in);
  buffer.str("");
  while (getline(ifile,line))
  {
    if (line.find("&general") != std::string::npos){break;}    
  }
  while (getline(ifile,line))
  {
    buffer << line << std::endl;
  }
  ifile.close();
  return buffer.str();
}

int get_traj_nframes(std::string trajfile,std::string prmtop)
{
  std::string n_frames;
  std::stringstream buffer;
  buffer.str("");
  //cpptraj -p topology-file.prmtop -y coordinate-file.coords -tl
  buffer << "cpptraj -p "<< prmtop << " -y " << trajfile << " -tl";
  n_frames = utils::GetSysResponse(buffer.str().c_str());
  return stoi(n_frames.substr(n_frames.find(":")+1,n_frames.size()-n_frames.find(":")-1));
}


FileList::FileList(int argc, char** argv)
{
    //parse arguments.
    for (int i=0; i < argc; i++)
    {
        if ((std::string)argv[i] == "-s")
        {
            solvated_prmtop = fs::absolute(argv[i+1]);
            i++;
            continue;
        }
        else if ((std::string)argv[i] == "-c")
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
    }
    // Validate Files.
    bool all_valid = true;
    if (!ValidateFile(mmpbsa_input))
    {
      all_valid = false;
    }
    if (!ValidateFile(trajectory))
    {
      all_valid = false;
    }
    if (!ValidateFile(solvated_prmtop))
    {
      all_valid = false;
    }
    if (!ValidateFile(complex_prmtop))
    {
      all_valid = false;
    }
    if (!ValidateFile(receptor_prmtop))
    {
      all_valid = false;
    }
    if (!ValidateFile(ligand_prmtop))
    {
      all_valid = false;
    }
    if (! all_valid)
    {
      std::cout << "Unable to validate all input files.  Exiting. " <<std::endl;
      exit(0);
    }
}

FileList::~FileList()
{
}

void FileList::copy_to_tmp()
{
    fs::copy_file(mmpbsa_input,"/tmp/mmpbsa.in");
    fs::copy_file(solvated_prmtop,"/tmp/solvated.prmtop");
    fs::copy_file(trajectory,"/tmp/trajectory.mdcrd");
    fs::copy_file(complex_prmtop, "/tmp/complex.prmtop");
    fs::copy_file(receptor_prmtop, "/tmp/receptor.prmtop");
    fs::copy_file(ligand_prmtop, "/tmp/ligand.prmtop");
}