#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>


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

// Get's process ids (uses std::filesystem)
vector<int> LinuxParser::Pids() {
  vector<int> pids;

  string folderPath = kProcDirectory.c_str();
  for(const auto& entry : std::filesystem::directory_iterator(folderPath)){
    if(entry.is_directory()){
      auto filename = entry.path().filename();
       auto name = filename.stem().string();
        if(std::all_of(name.begin(), name.end(), isdigit)){
          int pid  = stoi(name);
          pids.emplace_back(pid);
        }
    }
  };

  // DIR* directory = opendir(kProcDirectory.c_str());
  // struct dirent* file;
  // while ((file = readdir(directory)) != nullptr) {
  //   // Is this a directory?
  //   if (file->d_type == DT_DIR) {
  //     // Is every character of the name a digit?
  //     string filename(file->d_name);
  //     if (std::all_of(filename.begin(), filename.end(), isdigit)) {
  //       int pid = stoi(filename);
  //       pids.push_back(pid);
  //     }
  //   }
  // }
  // closedir(directory);
  return pids;
}

// Returns the system memory utilization
float LinuxParser::MemoryUtilization() { 
  int count = 0;
  long totalMemory;
  long availableMemory;

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
        else if(key == "MemAvailable:"){
          availableMemory = value;
          count++;
        }
    
      }
      if(count == 2){
        break;
      }
    }
  }
  
  long totalUsedMemory = totalMemory - availableMemory;
 

  float percentage = ((float)totalUsedMemory/totalMemory);
  return percentage; 
  
  }

// Returns the system uptime
long LinuxParser::UpTime() { 
    string line;
    long uptime;
    std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> uptime;
      
    }
    return uptime; 
  }

// Get process CPU usage
float LinuxParser::ProcessCpuUsage(int pid){


  string line;
  string unusedValue;
  long utime; // cpu time spent in the user code.
  long stime; // cpu time spent in the kernel.
  long cutime; // waited for children cpu time spent in user code.
  long cstime; // waited for children cpu time spent in kernel coce.

  long uptime = UpTime(pid);

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
  
 
  }
  // total time spent for the process
  long totalTime = utime + stime + cutime + cstime;

  // total time in seconds
  long totalTimeInSeconds = totalTime /sysconf(_SC_CLK_TCK);


  //cpu usage in percentage
  float usage =  (float)totalTimeInSeconds /uptime;

  return usage;
  
}

// Returns the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line;
  string cpu;
  long value;
  long jiffies = 0;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if(filestream.is_open()){
     while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> cpu;
      if(cpu == "cpu"){
        while(linestream >> value){
          jiffies += value;
        }
        break;
      }
    }
  }
  return jiffies; 
  
  }

// Returns the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  // process active jiffies = utime + stime + cutime + cstime
  string line;
  string value;
  int count = 1;
  long activeJiffies = 0;
  std::ifstream filestream(kProcDirectory+"/"+to_string(pid)+kStatFilename);
  if(filestream.is_open()){
      std::getline(filestream,line);
    std::istringstream linestream(line);
    while(linestream >> value){
      if(count == 14)
      {
        break;
      }
      count++;
    }
    while(linestream >> value){
      activeJiffies += std::stol(value);
      if(count == 18){
        break;
      }
      count++;
    }
  }
  return activeJiffies; 
  }

// Returns the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line;
  string value;
  long activeJiffies = 0;
  int count = 0;

  std::ifstream filestream(kProcDirectory+kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      
        if(count >= 1 && count <= 3){
          // user + nice + system
          activeJiffies += stol(value);
        }else if(count >= 5 && count <= 8){
          // irq + softirq + steal
          activeJiffies += stol(value);
        }
      count++;
    }
  }
  return activeJiffies; 
}

// Returns the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line;
  string value;
  long idleJiffies;
  int count = 0;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
        if(count == 4){
          idleJiffies += stol(value);
        }else if(count == 5){
          idleJiffies += stol(value);
          break;
        }
      count++;
    }
  }
  
  return idleJiffies; 
}

// Returns CPU utilization
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

// Returns the total number of processes
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

// Returns the number of running processes
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

// Returns the command associated with a process
string LinuxParser::Command(int pid) { 
    string cmd;
    std::ifstream stream(kProcDirectory+"/"+to_string(pid)+ kCmdlineFilename);
    if (stream.is_open()) {
      std:: getline(stream, cmd);
    }
  return cmd;
}

// Returns the memory used by a process
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
    // divide by 1024 to convert to megabytes
  return to_string(std::stol(value)/1024);
}

// Returns the user ID associated with a process
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

// Returns the user associated with a process
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

// Returns the uptime of a process
long LinuxParser::UpTime(int pid) { 
    string line;
    string value;
    long clockTicks;
    int count = 1;
    long systemTime = UpTime();
    std::ifstream stream(kProcDirectory+"/"+to_string(pid)+ kStatFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
        std::istringstream linestream(line);
        while(linestream >> value){
          if(count == 22){
           clockTicks = std::stol(value);
           break;
          }
          count++;
        }
      
    }
  return (systemTime - clockTicks/sysconf(_SC_CLK_TCK));
}
