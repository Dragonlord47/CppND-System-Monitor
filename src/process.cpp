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

// returns process identification number
int Process::Pid() { return processId_; }

// returns process cpu utilization
float Process::CpuUtilization() const{ 
    return LinuxParser::ProcessCpuUsage(processId_);
 }

// returns process command
string Process::Command() { return LinuxParser::Command(processId_)...; }

// returns process memory usage in megabytes
string Process::Ram() { return LinuxParser::Ram(processId_); }

// returns process owner
string Process::User() { return LinuxParser::User(processId_); }

// returns process uptime
long int Process::UpTime() { return LinuxParser::UpTime(processId_); }

// Sort processes in descending order
bool Process::operator<(Process const& a) const { 
   
  return this->CpuUtilization() > a.CpuUtilization() ? true : false;
}