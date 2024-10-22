// BANNG (c) by Borna Bešić
// 
// BANNG is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

#pragma once

#include <memory>
#include <cassert>
#include <iostream>
#include <functional>
#include <immintrin.h>

#include <memory.hpp>

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

    static T l2_distance(const Array1d<T> &a1, const Array1d<T> &a2);
};

template <typename T>
T Array1d<T>::l2_distance(const Array1d<T> &a1, const Array1d<T> &a2) {
    assert(a1.length == a2.length);
    T distance = 0;
    for (unsigned int i = 0; i < a1.length; ++i) {
        T delta = a1(i) - a2(i);
        distance += delta * delta;
    }
    return distance;
}

template <>
float Array1d<float>::l2_distance(const Array1d<float> &a1, const Array1d<float> &a2) {
    assert(a1.length == a2.length);
    const unsigned int rounds = a1.length / 8;

    float distance = 0;

    // Blocks
    unsigned int offset = 0;
    __m256 total = {0};
    __m256 delta;
    for (unsigned int round = 0; round < rounds; ++round) {
        delta = _mm256_sub_ps(
            _mm256_loadu_ps(a1.data + offset),
            _mm256_loadu_ps(a2.data + offset)
        );
        total = _mm256_add_ps(total, _mm256_mul_ps(delta, delta));
        offset += 8;
    }

    // Horizontal addition
    std::array<float, 8> result;
    _mm256_storeu_ps(result.data(), total);
    for (const auto &e : result) distance += e;

    // Rest
    while (offset < a1.length) {
        float delta = a1(offset) - a2(offset);
        distance += delta * delta;
        ++offset;
    }

    return distance;
}

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
    std::shared_ptr<Memory<T>> data;
    unsigned int rows, cols;
    unsigned int stride_rows, stride_cols;

    T &operator()(unsigned int i, unsigned int j) {
        return (*data)[stride_rows * i + stride_cols * j];
    }

    T operator()(unsigned int i, unsigned int j) const {
        return (*data)[stride_rows * i + stride_cols * j];
    }

    Array1d<T> operator()(unsigned int i) const {
        return {*data + stride_rows * i, cols, stride_cols};
    }

    static Array2d<T> allocate(unsigned int rows, unsigned int cols) {
        return {std::make_shared<RAM<T>>(rows * cols), rows, cols, cols, 1};
    }

    static Array2d<T> allocate(unsigned int rows, unsigned int cols, const std::string &filename) {
        return {std::make_shared<Disk<T>>(rows * cols, filename), rows, cols, cols, 1};
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
