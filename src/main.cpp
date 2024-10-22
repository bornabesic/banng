// BANNG (c) by Borna Bešić
// 
// BANNG is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

#include <chrono>
#include <iostream>

#include <array.hpp>
#include <index.hpp>
#include <random.hpp>
#include <time.hpp>

constexpr unsigned int n = 10000;
constexpr unsigned int d = 8;

inline Array1d<float> nearest_neighbor(const Array2d<float> &data, const Array1d<float> &query) {
    assert(data.cols == query.length);

    Array1d<float> nearest;
    float l2_squared_best = -1;
    for (unsigned int i = 0; i < data.rows; ++i) {
        float l2_squared = Array1d<float>::l2_distance(data(i), query);
        if (l2_squared_best == -1 || l2_squared < l2_squared_best) {
            nearest = data(i);
            l2_squared_best = l2_squared;
        }
    }

    return nearest;
}

template <template <typename> class S>
float calculate_ann_accuracy(const Array2d<float> &array, Index<float, S> &index, const unsigned int K = 100) {
    unsigned int correct = 0;
    std::vector<float> query_data(d);
    Array1d<float> query{query_data.data(), d, 1};
    for (unsigned int k = 0; k < K; ++k) {
        for (unsigned int i = 0; i < d; ++i) query_data[i] = random_normal();
        Array1d<float> nn_real = nearest_neighbor(array, query);
        Array1d<float> nn = index.search(query);
        correct += all_close(nn_real, nn) ? 1 : 0;
    }
    return ((float) correct) / K;
}

int main(void) {
    Array2d<float> array = Array2d<float>::allocate(n, d);

    Stopwatch stopwatch;

    stopwatch.checkpoint();
    for (unsigned int i = 0; i < n; ++i)
        for (unsigned int j = 0; j < d; ++j)
            array(i, j) = random_normal();
    std::cout << "Random data generation for " << n << " x " << d << " array took " << stopwatch.checkpoint() << " s" << '\n';
    std::cout << n * d * sizeof(float) / 1024.f / 1024.f << " MB" << '\n';

    Index<float, AxisAlignedSplit> index;
    stopwatch.checkpoint();
    index.build(array);
    std::cout << "Building the index took " << stopwatch.checkpoint() << " s" << '\n';

    stopwatch.checkpoint();
    float accuracy = calculate_ann_accuracy(array, index);
    std::cout << "Calculating accuracy took " << stopwatch.checkpoint() << " s" << '\n';
    std::cout << "Accuracy: " << accuracy * 100.f << " %" << '\n';

    return 0;
}