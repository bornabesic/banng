
#include <random>
#include <random.hpp>

float random_normal(float mean, float standard_deviation) {
    static std::default_random_engine generator;
    static std::normal_distribution<float> distribution(mean, standard_deviation);
    return distribution(generator);
}