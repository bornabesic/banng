#pragma once

#include <cassert>
#include <iostream>
#include <functional>
#include <immintrin.h>


// --------------------------------- Printing ---------------------------------

using std::placeholders::_1;
auto print_char = std::bind(std::putc, _1, stdout);

// --------------------------------- 1D Array ---------------------------------

template <typename T>
struct Array1d {
    T *data;
    unsigned int length;
    unsigned int stride;

    // T &operator()(unsigned int i) {
    //     return data[stride * i];
    // }

    T operator()(unsigned int i) const {
        return data[stride * i];
    }

    static void print(const Array1d<T> &array, const char *format = "%.4f") {
        for (unsigned int i = 0; i < array.length; ++i) {
            std::printf(format, array(i));
            print_char(' ');
        }
        print_char('\n');
    }

    static T l2_distance(const Array1d<T> &a1, const Array1d<T> &a2) {
        assert(a1.length == a2.length);
        const unsigned int rounds = a1.length / 8;
        const unsigned int left = a1.length % 8;

        T distance = 0;
        std::array<T, 8> result;
        unsigned int i = 0;
        for (unsigned int round = 0; round < rounds; ++round) {
            __m256 v1 = _mm256_loadu_ps(a1.data + i);
            __m256 v2 = _mm256_loadu_ps(a2.data + i);
            __m256 v = _mm256_sub_ps(v1, v2);
            v = _mm256_mul_ps(v, v);

            _mm256_storeu_ps(result.data(), v);
            for (const auto &e : result) distance += e;
            i += 8;
        }

        for (unsigned int i = a1.length - 1; i > a1.length - left; --i) {
            float delta = a1(i) - a2(i);
            distance += delta * delta;
        }

        return distance;
    }
};

template <typename T>
bool all_close(const Array1d<T> &v1, const Array1d<T> &v2, T eps = 1e-6) {
    assert(v1.length == v2.length);
    for (unsigned int i = 0; i < v1.length; ++i)
        if (v1(i) - v2(i) > eps)
            return false;
    return true;
}

// --------------------------------- 2D Array ---------------------------------

template <typename T>
struct Array2d {
    T *data;
    unsigned int rows, cols;
    unsigned int stride_rows, stride_cols;

    T &operator()(unsigned int i, unsigned int j) {
        return data[stride_rows * i + stride_cols * j];
    }

    T operator()(unsigned int i, unsigned int j) const {
        return data[stride_rows * i + stride_cols * j];
    }

    Array1d<T> operator()(unsigned int i) const {
        return {data + stride_rows * i, cols, stride_cols};
    }

    static Array2d<T> allocate(unsigned int rows, unsigned int cols) {
        T *data = new T[rows * cols];
        Array2d<T> array{data, rows, cols, cols, 1};

        assert(&array(rows - 1, cols - 1) - &array(0, 0) + 1 == rows * cols);
        return array;
    }

    static void free(const Array2d<T> &array) {
        delete[] array.data;
    }

    static void print(const Array2d<T> &array, const char *format = "%.4f") {
        for (unsigned int i = 0; i < array.rows; ++i) {
            for (unsigned int j = 0; j < array.cols; ++j) {
                std::printf(format, array(i, j));
                print_char(' ');
            }
            print_char('\n');
        }
    }
};
