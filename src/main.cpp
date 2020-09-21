
#include <chrono>
#include <iostream>

#include <array.hpp>
#include <index.hpp>
#include <random.hpp>
#include <time.hpp>

int main(void) {
    constexpr unsigned int n = 1000000;
    constexpr unsigned int d = 32;
    struct array_2d<float> array = allocate_array_2d<float>(n, d);

    Stopwatch stopwatch;

    stopwatch.checkpoint();
    for (unsigned int i = 0; i < n * d; ++i)
        array.data[0][i] = random_normal();
    std::cout << "Random data generation for " << n << " x " << d << " array took " << stopwatch.checkpoint() << " s" << '\n';
    std::cout << n * d * sizeof(float) / 1024.f / 1024.f << " MB" << '\n';

    stopwatch.checkpoint();
    index<float> *i = build_index(array);
    std::cout << "Building the index took " << stopwatch.checkpoint() << " s" << '\n';

    float query[d] = {0};
    array_1d<float> query_array = as_array_1d(query, d);
    std::cout << "Query:" << '\n';
    print_array_1d(query_array);

    stopwatch.checkpoint();
    array_1d<float> result_array = search_index(i, query_array);
    std::cout << "Querying the index took " << stopwatch.checkpoint() << " s" << '\n';
    std::cout << "Result:" << '\n';
    print_array_1d(result_array);

    free_index(i);
    free_array_2d(array);
    return 0;
}