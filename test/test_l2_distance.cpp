// BANNG (c) by Borna Bešić
// 
// BANNG is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

#include <gtest/gtest.h>
#include <array.hpp>

TEST(L2Distance, Test1) {
    float v1[2] = {0, 0};
    float v2[2] = {1, 1};

    Array1d<float> a1{v1, 2, 1};
    Array1d<float> a2{v2, 2, 1};

    float distance = Array1d<float>::l2_distance(a1, a2);
    EXPECT_EQ(distance, 2.f);
}