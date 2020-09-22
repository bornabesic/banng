#pragma once

#include <utility>
#include <vector>

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

    virtual bool descend_left(const array_1d<T> &query) const = 0;
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
        for (unsigned int i = 0; i < n; ++i)
            sum += data[i][this->axis];
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

    bool descend_left(const array_1d<T> &query) const {
        return (query.data[this->axis] < this->value);
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
        : split(nullptr), data(data), left(nullptr), right(nullptr),
          is_leaf(true) {}
    Node(Split<T> *split, Node<T> *left, Node<T> *right)
        : split(split), data(nullptr), left(left), right(right),
          is_leaf(false) {}
};

template <typename T, template <typename> typename S>
class Index {
  static_assert(std::is_base_of<Split<T>, S<T>>::value, "Class S<T> should be an instance of Split<T>!");

  private:
    Node<T> *tree;

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

    array_1d<T> _search_tree(const array_1d<T> &query, const Node<T> *tree) const {
        if (tree->is_leaf)
            return as_array_1d(tree->data, query.length);

        if (tree->split->descend_left(query))
            return _search_tree(query, tree->left);
        else
            return _search_tree(query, tree->right);
    }

  public:
    Index() : tree(nullptr) {}

    void build(const array_2d<T> &array) {
        this->tree = this->_build_tree(array.data, array.rows, array.cols);
    }

    array_1d<T> search(const array_1d<T> &query) const {
        return this->_search_tree(query, this->tree);
    }
};

