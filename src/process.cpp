#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): 
pid_(pid),
utilization_(Process::CpuUtilization()),
ram_(LinuxParser::Ram(pid))
{}

int Process::Pid() 
{ 
    return pid_; 
}

double Process::CpuUtilization() 
{
    const double currActv = LinuxParser::ActiveJiffies(pid_);
    const double currUp = LinuxParser::UpJiffies(pid_);
    
    if (currUp > 0) 
        return currActv/currUp;
    else 
        return 0; 
}

string Process::Command() 
{ 
    return LinuxParser::Command(pid_); 
}

string Process::Ram() 
{ 
    ram_ = LinuxParser::Ram(pid_); 
    return ram_;
}

string Process::User() 
{ 
    return LinuxParser::User(pid_); 
}

long Process::UpTime() 
{ 
    return (long int)LinuxParser::UpJiffies(pid_)/sysconf(_SC_CLK_TCK); 
}

bool Process::operator<(Process const& a) const { 
    return std::stof(this->ram_) < std::stof(a.ram_);
}