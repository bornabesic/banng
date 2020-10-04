// BANNG (c) by Borna Bešić
// 
// BANNG is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

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