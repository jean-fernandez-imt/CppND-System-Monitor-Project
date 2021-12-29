#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) 
{
    std::stringstream hh;
    hh << std::setw(2) << std::setfill('0') << (seconds/3600);
    std::stringstream mm;
    mm << std::setw(2) << std::setfill('0') << ((seconds%3600)/60);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << (seconds%60);
    return hh.str() + ":" + mm.str() + ":" + ss.str();
}