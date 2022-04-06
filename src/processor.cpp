#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
    long active = LinuxParser::ActiveJiffies();
    long idle = LinuxParser::IdleJiffies();
    float usage = ((float) active - previous_active_)/(idle + active - previous_active_ - previous_idle_);
    previous_active_ = active;
    previous_idle_ = idle;
    return usage;
}