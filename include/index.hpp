#pragma once

#include <utility>
#include <vector>
#include <cmath>
#include <memory>

#include <array.hpp>

template <typename T>
class Split {
  protected:
    unsigned int dimensionality;
    unsigned int depth;

  public:
    Split(unsigned int dimensionality, unsigned int depth)
        : dimensionality(dimensionality), depth(depth) {}

    virtual std::pair<std::vector<T *> *, std::vector<T *> *>
    split(T **data, const unsigned int n, const unsigned int d) = 0;

    virtual bool descend_left(const Array1d<T> &query) const = 0;

    virtual T distance_to_boundary(const Array1d<T> &query) const = 0;
};

template <typename T>
class AxisAlignedSplit : public Split<T> {
  private:
    unsigned int axis;
    T value;

  public:
    AxisAlignedSplit(unsigned int dimensionality, unsigned int depth)
        : Split<T>(dimensionality, depth) {
        this->axis = depth % dimensionality;
        this->value = 0;
    }

    std::pair<std::vector<T *> *, std::vector<T *> *>
    split(T **data, const unsigned int n, const unsigned int d) override {
        T sum = 0;
        {
            T max_spread = 0;
            for (unsigned int j = 0; j < d; ++j) {
                T min = data[0][j];
                T max = min;
                T axis_sum = 0;
                for (unsigned int i = 0; i < n; ++i) {
                    if (data[i][j] < min) {
                        min = data[i][j];
                    }
                    else if (data[i][j] > max) {
                        max = data[i][j];
                    }
                    axis_sum += data[i][j];
                }
                T spread = max - min;
                if (spread > max_spread) {
                    max_spread = spread;
                    sum = axis_sum;
                    this->axis = j;
                }
            }
        }

        // for (unsigned int i = 0; i < n; ++i)
        //     sum += data[i][this->axis];
        T mean = sum / n;
        this->value = mean;

        std::vector<T *> *left = new std::vector<T *>();
        std::vector<T *> *right = new std::vector<T *>();
        left->reserve(n / 2 + 1);
        right->reserve(n / 2 + 1);

        for (unsigned int i = 0; i < n; ++i) {
            std::vector<T *> *destination =
                (data[i][this->axis] < mean) ? left : right;
            destination->emplace_back(data[i]);
        }

        left->shrink_to_fit();
        right->shrink_to_fit();
        return {left, right};
    }

    bool descend_left(const Array1d<T> &query) const override {
        return (query(this->axis) < this->value);
    }

    T distance_to_boundary(const Array1d<T> &query) const override {
        return std::abs(query(this->axis) - this->value);
    }
};

template <typename T>
class Node {
  public:
    Split<T> *split;
    Node<T> *left;
    Node<T> *right;
    bool is_leaf;
    T *data;

    Node(T *data)
        : split(nullptr), left(nullptr), right(nullptr), is_leaf(true),
          data(data) {}
    Node(Split<T> *split, Node<T> *left, Node<T> *right)
        : split(split), left(left), right(right), is_leaf(false),
          data(nullptr) {}
};

template <typename T, template <typename> class S>
class Index {
  static_assert(std::is_base_of<Split<T>, S<T>>::value, "Class S<T> should be an instance of Split<T>!");

  private:
    Node<T> *tree;
    unsigned int stride;

    Node<T> *_build_tree(T **data, const unsigned int n, const unsigned int d,
                         const unsigned int depth = 0) {
        if (n < 2) {
            assert(n == 1);
            return new Node<T>(*data);
        }

        Split<T> *split = new S<T>(d, depth);
        auto subspaces = split->split(data, n, d);
        Node<T> *node = new Node<T>(
            split, this->_build_tree(subspaces.first->data(),
                                     subspaces.first->size(), d, depth + 1),
            this->_build_tree(subspaces.second->data(),
                              subspaces.second->size(), d, depth + 1));
        delete subspaces.first;
        delete subspaces.second;
        return node;
    }

    void _search_tree(const Array1d<T> &query, const Node<T> *tree, std::shared_ptr<Array1d<T>> &nearest, std::shared_ptr<float> &max_distance) const {
        if (tree->is_leaf) {
            std::shared_ptr<Array1d<T>> target(new Array1d<T>{tree->data, query.length, this->stride});
            const float distance = Array1d<T>::l2_distance(query, *target);
            if (max_distance.get() == nullptr) {
                max_distance = std::make_shared<T>(distance);
                nearest = target;
            }
            else if (distance < *max_distance) {
                *max_distance = distance;
                nearest = target;
            }
            return;
        }

        const bool dl = tree->split->descend_left(query);
        const Node<T> *destination = dl ? tree->left : tree->right;
        const Node<T> *other = !dl ? tree->left : tree->right;

        _search_tree(query, destination, nearest, max_distance);
        auto distance = tree->split->distance_to_boundary(*nearest);
        distance *= distance;
        if (distance < *max_distance) {
            std::shared_ptr<T> max_distance_potential(nullptr);
            std::shared_ptr<Array1d<T>> potential_nearest(nullptr);
            _search_tree(query, other, potential_nearest, max_distance_potential);
            if (*max_distance_potential < *max_distance) {
                *max_distance = *max_distance_potential;
                nearest = potential_nearest;
            }
        }
    }

  public:
    Index() : tree(nullptr) {}

    void build(const Array2d<T> &array) {
        std::vector<T *> row_ptrs(array.rows);
        for (unsigned int i = 0; i < array.rows; ++i)
            row_ptrs[i] = array(i).data;

        this->tree = this->_build_tree(row_ptrs.data(), array.rows, array.cols);
        this->stride = array.stride_cols;
    }

    Array1d<T> search(const Array1d<T> &query) const {
        std::shared_ptr<T> max_distance(nullptr);
        std::shared_ptr<Array1d<T>> nearest(nullptr);
        this->_search_tree(query, this->tree, nearest, max_distance);
        return *nearest;
    }
};

