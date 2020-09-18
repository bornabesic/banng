

#include <array.hpp>
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

    print_array_2d(array);
    free_array_2d(array);
    return 0;
}