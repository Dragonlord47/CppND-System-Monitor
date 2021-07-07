#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;


Process::Process(int id): processId_(id){}

// TODO: Return this process's ID
int Process::Pid() { return processId_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const{ 
    return LinuxParser::ProcessCpuUsage(processId_);
    
 }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(processId_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(processId_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(processId_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(processId_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
   
  return this->CpuUtilization() > a.CpuUtilization() ? true : false;
}