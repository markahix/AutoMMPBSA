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

int get_traj_nframes(std::string trajfile, std::string prmtop)
{
  std::string n_frames;
  std::stringstream buffer;
  buffer.str("");
  //cpptraj -p topology-file.prmtop -y coordinate-file.coords -tl
  buffer << "cpptraj -p "<< prmtop << " -y " << trajfile << " -tl";
  n_frames = GetSysResponse(buffer.str().c_str());
  return stoi(n_frames.substr(n_frames.find(":")+1,n_frames.size()-n_frames.find(":")-1));
}
