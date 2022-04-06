#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   // Completed in src/system.cpp
  std::vector<Process>& Processes();  // Completed in src/system.cpp
  float MemoryUtilization();          // Completed in src/system.cpp
  long UpTime();                      // Completed in src/system.cpp
  int TotalProcesses();               // Completed in src/system.cpp
  int RunningProcesses();             // Completed in src/system.cpp
  std::string Kernel();               // Completed in src/system.cpp
  std::string OperatingSystem();      // Completed in src/system.cpp

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};

};

#endif