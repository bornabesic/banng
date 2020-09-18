

#include <array.hpp>
#include <index.hpp>
#include <iostream>

int main(void) {
    int n = 3;
    int d = 2;
    struct array_2d<float> array = allocate_array_2d<float>(n, d);

    array.data[0][0] = 1;
    array.data[0][1] = 2;
    //
    array.data[1][0] = 3.14;
    array.data[1][1] = 4;
    //
    array.data[2][0] = 5;
    array.data[2][1] = 6;

    std::cout << "Data:" << '\n';
    print_array_2d(array);

    index<float> *i = build_index(array);

    float query[2] = {0};
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