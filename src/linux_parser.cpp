#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  int count = 0;
  float totalMemory;
  float freeMemory;
  float availableMemory;
  int value;
  string line;
  string key;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if(filestream.is_open()){
    while(std::getline(filestream,line)){
      std::istringstream linestream(line);
      while(linestream >> key >>value){
        if(key == "MemTotal:"){
          totalMemory = value;
          count++;
          
        }
        else if(key == "MemFree:"){
          freeMemory = value;
          count++;
        }
        else if(key == "MemAvailable:"){
          availableMemory = value;
          count++;
        }
    
      }
      if(count == 3){
        break;
      }
    }
  }
  
  float usedMemory = totalMemory - availableMemory;
  float percentage = (usedMemory/totalMemory);
  return percentage; 
  
  }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
    string line;
    long runningAndSuspendTime;
    long idleTime;
    std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (stream.is_open()) {
      while(std::getline(stream, line)){
        std::istringstream linestream(line);
        linestream >> runningAndSuspendTime >> idleTime;
        break;
      }
    }
    return runningAndSuspendTime + idleTime; 
  }

// Get process CPU usage
float LinuxParser::ProcessCpuUsage(int pid){

  string line;
  string unusedValue;
  long utime; // cpu time spent in the user code.
  long stime; // cpu time spent in the kernel.
  long cutime; // waited for children cpu time spent in user code.
  long cstime; // waited for children cpu time spent in kernel coce.
  long starttime; // time when the process started.
  long uptime = UpTime();

  std::ifstream stream(kProcDirectory+"/"+to_string(pid)+kStatFilename);
  if(stream.is_open()){
  std::getline(stream, line);
  std::istringstream linestream(line);
  int count = 1;
  
   while(linestream >> unusedValue){
      count++;
      if(count == 14)
       break;
   };
    
  
  linestream >> utime >> stime >>  cutime >> cstime;
  count = 18;
    while(linestream >> unusedValue){
      count++;
      if(count == 22)
       break;
    };
    
  
   linestream >> starttime;
 
  }
  // total time spent for the process
  long totalTime = utime + stime + cutime + cstime;

  // total time in seconds
  //long totalTimeInSeconds = totalTime /sysconf(_SC_CLK_TCK);

  // total elasped time in seconds since the process started
  long elaspedTimeInSeconds = uptime - (starttime /sysconf(_SC_CLK_TCK));

  //cpu usage in percentage
  float usage =  (totalTime/sysconf(_SC_CLK_TCK)) * 1.0 /elaspedTimeInSeconds * 1.0;

  return usage;
  
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuData;
    string line;
    string cpu;
    string value;

    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()) {
        while(std::getline(stream, line)){
        std::istringstream linestream(line);
        linestream >> cpu;
        if(cpu == "cpu"){
          while(linestream >> value){
            cpuData.emplace_back(value);
          }
          break;
        }
     }
    }
  return cpuData;
   }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    while(std::getline(filestream,line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "processes"){
          return value;
        }
      }
    }
  }
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
    string line;
    string key;
    int value;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if(filestream.is_open()){
      while(std::getline(filestream,line)){
        std::istringstream linestream(line);
        while(linestream >> key >> value){
          if(key == "procs_running"){
            return value;
          }
        }
      }
    }
    return 0; 
   
   }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
    string cmd;
    std::ifstream stream(kProcDirectory+"/"+to_string(pid)+ kCmdlineFilename);
    if (stream.is_open()) {
      std:: getline(stream, cmd);
    }
  return cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
    string line;
    string value;
    string key;
    std::ifstream stream(kProcDirectory+"/"+to_string(pid)+ kStatusFilename);
    if (stream.is_open()) {
      while(std::getline(stream, line)){
        std::istringstream linestream(line);
        linestream >> key >> value;
        if(key == "vmSize:"){
          break;
        }
      }
    }
  return to_string(std::stol(value)/1024);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
    string line;
    string value;
    string key;
    std::ifstream stream(kProcDirectory+"/"+to_string(pid)+ kStatusFilename);
    if (stream.is_open()) {
      while(std::getline(stream, line)){
        std::istringstream linestream(line);
        linestream >> key >> value;
        if(key == "Uid:"){
          break;
        }
      }
    }
  return value;
 }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
    string uid = Uid(pid);
    string line;
    string username;
    string x;
    string uid1;
    string uid2;
    string key;
    std::ifstream stream(kPasswordPath);
    if (stream.is_open()) {
      while(std::getline(stream, line)){
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> username >> x >> uid1 >> uid2;
        if(uid == uid1 && uid == uid2){
          break;
        }
      }
    }
  return username;
 }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
    string line;
    string value;
    long clockTicks;
    int count = 0;
    std::ifstream stream(kProcDirectory+"/"+to_string(pid)+ kStatFilename);
    if (stream.is_open()) {
      while(std::getline(stream, line)){
        std::istringstream linestream(line);
        while(linestream >> value){
          count++;
          if(count == 22){
           std::string::size_type size;
           clockTicks = std::stol(value,&size);
           break;
          }
        }
        break;
      }
    }
  return clockTicks/sysconf(_SC_CLK_TCK);
}
