#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

using namespace std;


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    vector<string> processorData = LinuxParser::CpuUtilization();
   
    long user = atoi(processorData[0].c_str());
    long nice = atoi(processorData[1].c_str());
    long system = atoi(processorData[2].c_str());
    long idle = atoi(processorData[3].c_str());
    long iowait = atoi(processorData[4].c_str());
    long irq = atoi(processorData[5].c_str());
    long softirq = atoi(processorData[6].c_str());
    long steal = atoi(processorData[7].c_str());
    long guest = atoi(processorData[8].c_str());
    long guestNice = atoi(processorData[9].c_str());

    long usertime = user - guest;
    long nicetime = nice - guestNice;

    long idletime = idle + iowait;
    long systemtime = system + irq + softirq;
    long virtualtime = guest + guestNice;
    long  totaltime = usertime + nicetime + systemtime + idletime + steal + virtualtime;

    float percentage = ((totaltime - idletime) * 1.0/ totaltime * 1.0) ;

    return percentage; 
    
}