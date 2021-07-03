#include <string>

#include "format.h"

using std::string;
using std::to_string;
// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int totalMinutes = seconds/60;
    int remainingSeconds = seconds % 60;
    int hours = totalMinutes / 60;
    int remainingMinutes = totalMinutes % 60;
    return to_string(hours)+":"+to_string(remainingMinutes)+":"+(remainingSeconds < 10 ? "0"+to_string(remainingSeconds).substr(0,1) :to_string(remainingSeconds));
    
    
}