#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {

 public:
  float Utilization();  // Completed in src/processor.cpp

  // TODO: Declare any necessary private members
 private:
    long previous_active_{0};
    long previous_idle_{0};

};

#endif