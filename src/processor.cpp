#include <string>
#include <vector>

#include "processor.h"
#include "linux_parser.h"

using std::string;
using std::vector;

float Processor::Utilization() 
{
    const long currActv = LinuxParser::ActiveJiffies();
    const long currIdle = LinuxParser::IdleJiffies();

    const long prevTotal = activeJiffies_ + idleJiffies_;
    const long currTotal = currActv + currIdle;

    const float totald = currTotal - prevTotal;
    const float idled = currIdle - idleJiffies_;

    activeJiffies_ = currActv;
    idleJiffies_ = currIdle;

    if (totald > 0) 
        return (totald - idled)/totald;
    else 
        return 0;
}