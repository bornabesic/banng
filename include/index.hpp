
#include <array.hpp>
#include <vector>

template <typename T>
struct split_node {

    split_node(T value, unsigned int axis, struct split_node<T> *left,
               struct split_node<T> *right)
        : value(value), axis(axis), left(left), right(right), data(nullptr),
          is_leaf(false){};

    split_node(T *data)
        : value(-1), axis(-1), left(nullptr), right(nullptr), data(data),
          is_leaf(true){};

    T value;
    unsigned int axis;
    struct split_node<T> *left;
    struct split_node<T> *right;
    T *data;
    bool is_leaf;
};

template <typename T>
struct split_node<T> *axis_aligned_split(T **data, const unsigned int n,
                                         const unsigned int d,
                                         const unsigned int axis) {
    if (n < 2) {
        assert(n == 1);
        return new struct split_node<T>(*data);
    }

    T sum = 0;
    for (unsigned int i = 0; i < n; ++i)
        sum += data[i][axis];
    T mean = sum / n;

    std::vector<T *> left, right;
    left.reserve(n / 2 + 1);
    right.reserve(n / 2 + 1);

    for (unsigned int i = 0; i < n; ++i) {
        std::vector<T *> &destination = (data[i][axis] < mean) ? left : right;
        destination.emplace_back(data[i]);
    }

    left.shrink_to_fit();
    right.shrink_to_fit();

    unsigned int next_axis = (axis + 1) % d;
    return new struct split_node<T>(
        mean,
        axis,
        axis_aligned_split(left.data(), left.size(), d, next_axis),
        axis_aligned_split(right.data(), right.size(), d, next_axis)
    );
}

template <typename T>
using index = split_node<T>;

template <typename T> index<T> *build_index(const struct array_2d<T> &array) {
    return axis_aligned_split(array.data, array.rows, array.cols, 0);
}

template <typename T>
array_1d<T> search_index(const index<T> *i, const array_1d<T> &query) {
    if (i->is_leaf)
        return as_array_1d(i->data, query.length);

    if (query.data[i->axis] < i->value)
        return search_index(i->left, query);
    else
        return search_index(i->right, query);
}

template <typename T>
void free_index(const index<T> *i) {
    delete i->left;
    delete i->right;
    delete i;
}
