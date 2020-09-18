#pragma once

#include <cassert>
#include <iostream>

template <typename T>
struct array_2d {
    T **data;
    unsigned int rows, cols;
};

template <typename T>
array_2d<T> allocate_array_2d(const unsigned int rows,
                              const unsigned int cols) {

    T *data = new T[rows * cols];
    T **row_ptrs = new T *[rows];
    for (int i = 0; i < rows; ++i)
        row_ptrs[i] = data + i * cols;

    assert(&row_ptrs[rows - 1][cols - 1] - &row_ptrs[0][0] + 1 == rows * cols);
    return {row_ptrs, rows, cols};
}

template <typename T>
void print_array_2d(const struct array_2d<T> &array) {
    for (int i = 0; i < array.rows; ++i) {
        for (int j = 0; j < array.cols; ++j)
            std::cout << array.data[i][j] << ' ';
        std::cout << '\n';
    }
}

template <typename T>
void free_array_2d(const struct array_2d<T> &array) {
    delete array.data[0];
    delete array.data;
}