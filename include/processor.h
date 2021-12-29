#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

#include "linux_parser.h"

class Processor 
{
public:
    float Utilization(); 

private:
    long activeJiffies_{LinuxParser::ActiveJiffies()};
    long idleJiffies_{LinuxParser::IdleJiffies()};
};

#endif