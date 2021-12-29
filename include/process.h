#ifndef PROCESS_H
#define PROCESS_H

#include "linux_parser.h"

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process 
{
public:
    Process(int pid);
    int Pid();                               
    std::string User();                      
    std::string Command();                   
    double CpuUtilization();                  
    std::string Ram();                       
    long UpTime();                         
    bool operator<(Process const& a) const;

private:
    int pid_;
    double utilization_;
    std::string ram_;
};

#endif