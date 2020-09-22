#pragma once

#include <cassert>
#include <iostream>

// --------------------------------- 2D Array ---------------------------------

template <typename T>
struct Array2d {
    T **data;
    unsigned int rows, cols;

    static Array2d<T> allocate(unsigned int rows, unsigned int cols) {
        T *data = new T[rows * cols];
        T **row_ptrs = new T *[rows];
        for (unsigned int i = 0; i < rows; ++i)
            row_ptrs[i] = data + i * cols;

        assert(&row_ptrs[rows - 1][cols - 1] - &row_ptrs[0][0] + 1 == rows * cols);
        return {row_ptrs, rows, cols};
    }

    static void free(const Array2d<T> &array) {
        delete array.data[0];
        delete array.data;
    }
};

template <typename T>
void print_array(const Array2d<T> &array) {
    for (unsigned int i = 0; i < array.rows; ++i) {
        for (unsigned int j = 0; j < array.cols; ++j)
            std::cout << array.data[i][j] << ' ';
        std::cout << '\n';
    }
}

// --------------------------------- 1D Array ---------------------------------

template <typename T>
struct Array1d {
    T *data;
    unsigned int length;
};

template <typename T>
void print_array(const Array1d<T> &array) {
    for (unsigned int i = 0; i < array.length; ++i)
        std::cout << array.data[i] << ' ';
    std::cout << '\n';
}

template <typename T>
bool all_close(const Array1d<T> &v1, const Array1d<T> &v2, T eps = 1e-6) {
    assert(v1.length == v2.length);
    for (unsigned int i = 0; i < v1.length; ++i)
        if (v1.data[i] - v2.data[i] > eps)
            return false;
    return true;
}