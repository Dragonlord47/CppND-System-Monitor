#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  double Utilization();  

  
 private:
    //previous cpu idle value
    long prevIdle_ = 0;
    //previous cpu non idle value
    long prevNonIdle_ = 0;
    //previous cpu total value
    long prevTotal_ = 0;
    
    
};

#endif