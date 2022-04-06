#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

// This can be used for print out of internal variables 
//#define TEST

// Used for debug printing
#ifdef TEST
#include <iostream>
#endif

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// When the processes terminate, there is a risk that the read data can be wrong
// These two help to avoid termination of the program due to incorrect inputs
long safe_stol(string l){
  long value = 0;
  try{
    value = std::stol(l);
  }
  catch (...)
  {}
  return value;
}

int safe_stoi(string l){
  int value = 0;
  try{
    value = std::stoi(l);
  }
  catch (...)
  {}
  return value;
}

// Done by Udacity as an example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line{""}, key{""}, value{""};
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

// Done by Udacity as an example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os{""}, version{""}, kernel{""}, line{""};
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids{};
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = safe_stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization: 1 - FreeMemory/TotalMemory
float LinuxParser::MemoryUtilization() { 
  string line{""}, key{""};
  long MemTotal{1000}, MemFree{0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    for (int i=0; i<2; i ++){
      std::getline(filestream, line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "MemTotal") {
        linestream >> MemTotal;
      } else if (key == "MemFree") {
        linestream >> MemFree;
      }
    }
    return 1.0 - (float)MemFree/MemTotal;
  }
  else {
    return 0;
  }
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line{0};
  long SystemUpTime{0};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> SystemUpTime;
  }
  return SystemUpTime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> all_jiffies = CpuUtilization();
  if (all_jiffies.size() > 0) {
    long value = 0;
    for ( string v : all_jiffies){
      value += safe_stol(v);

    }
    return value;
  }
  else {
    return 0;
  }
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line{""}, value{""};
  vector<string> all_jiffies{};
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      while (linestream >> value) {
        all_jiffies.push_back(value);
      }

    // Calculating active jiffies following https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    long utime = safe_stol(all_jiffies[13]);
    long stime = safe_stol(all_jiffies[14]);
    long cutime = safe_stol(all_jiffies[15]);
    long cstime = safe_stol(all_jiffies[16]);

    return utime + stime + cutime + cstime;
  }
  else {
    return 0;
  }
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line{""}, key{""};
  vector<string> all_jiffies = CpuUtilization();
  long value{0};
  if (all_jiffies.size() > 0) {
    for ( string v : all_jiffies){
      value += safe_stol(v);
    }
    return value - safe_stol(all_jiffies[CPUStates::kIdle_]);
  }
  else {
    return 0;
  }
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line{""}, key{""};
  vector<string> all_jiffies = CpuUtilization();
  return safe_stol(all_jiffies[CPUStates::kIdle_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line{""}, key{""}, value{""};
  vector<string> all_jiffies{};
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> key;
      while (linestream >> value) {
        all_jiffies.push_back(value);
      }
  }
  return all_jiffies;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line{""}, key{""}, value{""};
  vector<string> all_jiffies{};
  int processes{0};
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key ;
      if (key == "processes") {
        linestream >> processes;
        break;      
      }
    }
  }
  return processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()  {
  string line;
  string key;
  vector<string> all_jiffies;
  string value;
  int procs_running{0};
  std::ifstream filestream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key ;
      if (key == "procs_running") {
        linestream >> procs_running;
        break;      
      }
    }
  }
  return procs_running;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line{""};
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kCmdlineFilename);
  if (filestream.is_open()) {
      std::getline(filestream, line);
  }
  return line; 
}

// Read and return the memory used by a process
// Convert the output from KB to MB
string LinuxParser::Ram(int pid) { 
  string line{""}, key{""}, value{""}, ram{""};
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          ram = std::to_string(safe_stol(value)/1000);
          break;
        }
      }
    }
  }
  return ram;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line{""}, key{""}, value, uid{""};
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = value;
          break;
        }
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, key, value, uid = Uid(pid);
  std::ifstream filestream(LinuxParser::kPasswordPath);
  string username {""};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      while (linestream >> value >> key >> key) {
        if (key == uid) {
          username = value;
          break;
        }
      }
    }
  }
  return username;
}

// Read and return the uptime of a process
// Column #22, or index 21, is marked on the figure. According to https://man7.org/linux/man-pages/man5/proc.5.html
// it is process start time. Process uptime must then be system uptime - process start time. Below, it is calculated
// in jiffies.
long LinuxParser::UpTime(int pid) { 
  string line;
  vector<string> all_jiffies;
  string value;
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      all_jiffies.push_back(value);
    }
    return UpTime()*sysconf(_SC_CLK_TCK) - safe_stol(all_jiffies[21]);
  }
  else {
    return 0;
  }
}

// Added for calculation of the process CPU usage according to the scheme presented in 
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
// Read and return the start time of a process in seconds
long LinuxParser::StartTime(int pid) { 
  string line;
  vector<string> all_jiffies;
  string value;
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      all_jiffies.push_back(value);
    }
    return safe_stol(all_jiffies[21]);
  }
  else {
    return 0;
  }
}
