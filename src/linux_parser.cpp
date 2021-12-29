#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <unordered_map>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  string memType;
  float memVal;
  std::unordered_map<string, float> memData;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> memType >> memVal;
      memData.insert({memType, memVal});
    }
  }
  return
  (memData["MemTotal"]
  - memData["MemFree"])
  /memData["MemTotal"];
}

long LinuxParser::UpTime() {
  long int up_seconds;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::replace(line.begin(), line.end(), '.', ' ');
    std::istringstream linestream(line);
    linestream >> up_seconds;
  }
  return up_seconds;
}

long LinuxParser::Jiffies() { 
  return 
  LinuxParser::UpTime()
  *sysconf(_SC_CLK_TCK); 
}

long LinuxParser::ActiveJiffies(int pid) { 
  auto stat_data = LinuxParser::CpuUtilization(pid);
  return
  std::stol(stat_data[ProcStates::kUtime_])
  + std::stol(stat_data[ProcStates::kStime_])
  + std::stol(stat_data[ProcStates::kCUtime_])
  + std::stol(stat_data[ProcStates::kCStime_]);
}

long LinuxParser::ActiveJiffies() {
  auto cpu_data = LinuxParser::CpuUtilization(); 
  return 
  std::stol(cpu_data[CPUStates::kUser_])
  + std::stol(cpu_data[CPUStates::kNice_]) 
  + std::stol(cpu_data[CPUStates::kSystem_]) 
  + std::stol(cpu_data[CPUStates::kIRQ_]) 
  + std::stol(cpu_data[CPUStates::kSoftIRQ_])
  + std::stol(cpu_data[CPUStates::kSteal_])
  - std::stol(cpu_data[CPUStates::kGuest_])
  - std::stol(cpu_data[CPUStates::kGuestNice_]); 
}

long LinuxParser::IdleJiffies() { 
  auto cpu_data = LinuxParser::CpuUtilization();
  return 
  std::stol(cpu_data[CPUStates::kIdle_])
  + std::stol(cpu_data[CPUStates::kIOwait_]); 
}

vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string word;
  vector<string> parsed;
  vector<string> cpu_data;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
      while(linestream >> word) {
          parsed.push_back(word);
      }
  }
  cpu_data = vector<string>(parsed.begin() + 1, parsed.end());
  return cpu_data; 
}

vector<string> LinuxParser::CpuUtilization(int pid) { 
  string line;
  string word;
  vector<string> stat_data;
  std::ifstream filestream(kProcDirectory + to_string(pid) + "/" + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> word) {
          stat_data.push_back(word);
      }
    }
  }
  return stat_data; 
}

int LinuxParser::TotalProcesses() {  
  string line;
  string key;
  int val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "processes") {
        return val;
      } 
    }
  }
  return val;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int val;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "procs_running") {
        return val;
      }    
    }
  }
  return val;
}

string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + "/" + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return line; 
}

string LinuxParser::Ram(int pid) { 
  string line;
  string label;
  string data;
  std::ifstream filestream(kProcDirectory + to_string(pid) + "/" + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> label >> data;
      if (label == "VmSize") {
        std::stringstream dataMB;
        dataMB
        << std::fixed 
        << std::setprecision(1) 
        << (std::stof(data)/1024.0);
        return dataMB.str();
      }
    }
  }
  return data; 
}

string LinuxParser::Uid(int pid) { 
  string line;
  string label;
  string data;
  std::ifstream filestream(kProcDirectory + to_string(pid) + "/" + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> label >> data;
      if (label == "Uid") {
        return data;
      }
    }
  }
  return data; 
}

string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string line;
  string user;
  string mark;
  string id;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> mark >> id;
      if (id == uid) {
        return user;
      }
    }
  }
  return user;
}

long LinuxParser::UpJiffies(int pid) { 
  auto stat_data = LinuxParser::CpuUtilization(pid);
  return LinuxParser::Jiffies() - std::stol(stat_data[ProcStates::kStartTime_]); 
}
