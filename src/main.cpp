
#include <chrono>
#include <iostream>

#include <array.hpp>
#include <index.hpp>
#include <random.hpp>
#include <time.hpp>

constexpr unsigned int n = 10000;
constexpr unsigned int d = 8;

inline array_1d<float> nearest_neighbor(const array_2d<float> &data, const array_1d<float> &query) {
    assert(data.cols == query.length);

    float *nearest = nullptr;
    float l2_squared_best = -1;
    for (unsigned int i = 0; i < data.rows; ++i) {
        float l2_squared = 0;
        for (unsigned int j = 0; j < data.cols; ++j) {
            float delta = data.data[i][j] - query.data[j];
            l2_squared += delta * delta;
        }

        if (l2_squared_best == -1 || l2_squared < l2_squared_best) {
            nearest = data.data[i];
            l2_squared_best = l2_squared;
        }
    }

    return as_array_1d<float>(nearest, query.length);
}

template <template <typename> typename S>
float calculate_ann_accuracy(const array_2d<float> &array, Index<float, S> &index, const unsigned int K = 100) {
    unsigned int correct = 0;
    float *query_data = new float[d];
    array_1d<float> query = as_array_1d<float>(query_data, d);
    for (unsigned int k = 0; k < K; ++k) {
        for (unsigned int i = 0; i < d; ++i) query_data[i] = random_normal();
        array_1d<float> nn_real = nearest_neighbor(array, query);
        array_1d<float> nn = index.search(query);
        correct += all_close(nn_real, nn) ? 1 : 0;
    }
    delete query_data;
    return ((float) correct) / K;
}

int main(void) {
    array_2d<float> array = allocate_array_2d<float>(n, d);

    Stopwatch stopwatch;

    stopwatch.checkpoint();
    for (unsigned int i = 0; i < n * d; ++i)
        array.data[0][i] = random_normal();
    std::cout << "Random data generation for " << n << " x " << d << " array took " << stopwatch.checkpoint() << " s" << '\n';
    std::cout << n * d * sizeof(float) / 1024.f / 1024.f << " MB" << '\n';

    Index<float, AxisAlignedSplit> index;
    stopwatch.checkpoint();
    index.build(array);
    std::cout << "Building the index took " << stopwatch.checkpoint() << " s" << '\n';

    float accuracy = calculate_ann_accuracy(array, index);
    std::cout << "Accuracy: " << accuracy * 100.f << " %" << '\n';

    free_array_2d(array);
    return 0;
}