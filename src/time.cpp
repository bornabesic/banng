// BANNG (c) by Borna Bešić
// 
// BANNG is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

#include <chrono>

#include <time.hpp>

using std::chrono::high_resolution_clock;

Stopwatch::Stopwatch() : t_start(high_resolution_clock::now()) {}

float Stopwatch::checkpoint() {
    auto t_end = high_resolution_clock::now();
    std::chrono::duration<float> t = t_end - this->t_start;
    this->t_start = t_end;
    return t.count();
}