// BANNG (c) by Borna Bešić
// 
// BANNG is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

#include <random>
#include <random.hpp>

float random_normal(float mean, float standard_deviation) {
    static std::default_random_engine generator;
    static std::normal_distribution<float> distribution(mean, standard_deviation);
    return distribution(generator);
}