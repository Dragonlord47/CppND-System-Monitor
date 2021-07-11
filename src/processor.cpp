#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

using namespace std;


// Return the aggregate CPU utilization
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



    long currentIdle = idle + iowait;

    long currentNonIdle = user + nice + system + irq + softirq + steal;

    long currentTotal = currentIdle + currentNonIdle;

    long totalDiff = currentTotal - prevTotal_;
    long idleDiff = currentIdle - prevIdle_;

    float percentage = (float(totalDiff - idleDiff)/ totalDiff) ;

    prevIdle_ = currentIdle;
    prevNonIdle_ = currentNonIdle;
    prevTotal_ = currentTotal;

    return percentage; 
    
}