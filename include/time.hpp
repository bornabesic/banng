#pragma once

#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::seconds;

class Stopwatch {

  private:
    high_resolution_clock::time_point t_start;

  public:
    Stopwatch();
    float checkpoint();

};