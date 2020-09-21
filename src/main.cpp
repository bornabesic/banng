
#include <chrono>
#include <iostream>

#include <array.hpp>
#include <index.hpp>
#include <random.hpp>

int main(void) {
    constexpr unsigned int n = 1000000;
    constexpr unsigned int d = 32;
    struct array_2d<float> array = allocate_array_2d<float>(n, d);

    auto t_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n * d; ++i)
        array.data[0][i] = random_normal();
    std::chrono::duration<float> t = std::chrono::high_resolution_clock::now() - t_start;
    std::cout << "Allocation of " << n << " x " << d << " array took " << t.count() << " s" << '\n';
    std::cout << n * d * sizeof(float) / 1024.f / 1024.f << " MB" << '\n';

    index<float> *i = build_index(array);

    float query[d] = {0};
    array_1d<float> query_array = as_array_1d(query, d);
    std::cout << "Query:" << '\n';
    print_array_1d(query_array);

    array_1d<float> result_array = search_index(i, query_array);
    std::cout << "Result:" << '\n';
    print_array_1d(result_array);

    free_index(i);
    free_array_2d(array);
    return 0;
}