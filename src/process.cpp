#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

#include <unistd.h> 

#include "linux_parser.h"

// From https://stackoverflow.com/questions/1962482/hz-variable-not-defined, to get the value of HZ:
#include <asm-generic/param.h>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_{pid}
{
    user_ = LinuxParser::User(pid);
    command_ = LinuxParser::Command(pid);
    ram_ = LinuxParser::Ram(pid);
    uptime_ = LinuxParser::UpTime(pid);

// Calculation of the process CPU usage according to the scheme presented in 
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    long systemuptime = LinuxParser::UpTime();
    // To void problems with division by 0
    try {
       cpuusage_= ((float) LinuxParser::ActiveJiffies(pid)/sysconf(_SC_CLK_TCK))/(systemuptime - LinuxParser::StartTime(pid)/sysconf(_SC_CLK_TCK));
    } catch (...) {
        cpuusage_= 0;
    }
 };

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization()  const { return cpuusage_; }

// Return the command that generated this process
string Process::Command() const { return command_; }

// Return this process's memory utilization 
// Convert from KB to MB
string Process::Ram()  const { return ram_; }

// Return the user (name) that generated this process
string Process::User()  const { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime()  const { return uptime_/sysconf(_SC_CLK_TCK); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const
{ 
    // different criteria can be chosen
    return cpuusage_ < a.CpuUtilization();
    // return pid_ < a.Pid(); 
    // return uptime_ < a.UpTime();
}