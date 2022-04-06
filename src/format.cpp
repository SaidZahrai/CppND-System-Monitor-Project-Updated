#include <string>

#include <iomanip>

#include "format.h"

using std::string;

// Helper function to format time
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    long hours{0}, minutes{0};
    hours = seconds / 3600;
    minutes = (seconds % 3600) / 60;
    std::ostringstream stream;
    try {
        stream << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds%60;
    }
    catch (...) {
        stream << "00:00:00";
    }
    return stream.str();
}