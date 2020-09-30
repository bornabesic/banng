
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