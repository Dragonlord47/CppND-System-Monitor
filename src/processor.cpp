#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

using namespace std;


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    vector<string> processorData = LinuxParser::CpuUtilization();
   
    int user = atoi(processorData[0].c_str());
    int nice = atoi(processorData[1].c_str());
    int system = atoi(processorData[2].c_str());
    int idle = atoi(processorData[3].c_str());
    int iowait = atoi(processorData[4].c_str());
    int irq = atoi(processorData[5].c_str());
    int softirq = atoi(processorData[6].c_str());
    int steal = atoi(processorData[7].c_str());
    int guest = atoi(processorData[8].c_str());
    int guestNice = atoi(processorData[9].c_str());

    int usertime = user - guest;
    int nicetime = nice - guestNice;

    int idletime = idle + iowait;
    int systemtime = system + irq + softirq;
    int virtualtime = guest + guestNice;
    int totaltime = usertime + nicetime + systemtime + idletime + steal + virtualtime;

    float percentage = ((totaltime * 1.0 - idletime* 1.0 ))/ totaltime * 1.0;

    return percentage; 
    
}