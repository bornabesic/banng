# BANNG 💥
Borna's Approximate Nearest Neighbor Gearbox

# Requirements
- [C++11](https://en.cppreference.com/w/cpp/11)-compatible compiler
- [AVX](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions)-compatible CPU
- [CMake](https://cmake.org/)
- [Python](https://www.python.org/)
- [Linux-based OS](https://en.wikipedia.org/wiki/List_of_Linux_distributions)

# Building
```sh
git clone --recursive https://github.com/bornabesic/banng.git
cd banng
mkdir build
cd build
cmake .. -D CMAKE_BUILD_TYPE=Release
make
```

# Example
## Python
```python

import banng.float as banng
import numpy as np

n, d = 8, 2

X = np.random.uniform(size=(n, d)).astype(np.float32)
print("Data:")
print(X)
print()

index = banng.AxisAlignedSplit.Index()
index.build(X)

query = np.array([0] * d, dtype=np.float32)
print("Query:", query)
print("Result:", index.search(query))

```

## C++
```c++
#include <iostream>
#include <cstdlib>

/* BANNG */
#include <array.hpp>
#include <index.hpp>

int main(void) {
    constexpr unsigned int n = 8;
    constexpr unsigned int d = 2;

    Array2d<float> X = Array2d<float>::allocate(n, d);
    for (unsigned int i = 0; i < n; ++i)
        for (unsigned int j = 0; j < d; ++j)
            X(i, j) = ((float) std::rand()) / RAND_MAX;

    Index<float, AxisAlignedSplit> index;
    index.build(X);

    std::cout << "Data:" << '\n';
    Array2d<float>::print(X);
    std::cout << '\n';

    float query_data[d] = {0};
    constexpr unsigned int query_stride = 1;
    Array1d<float> query{query_data, d, query_stride};
    std::cout << "Query: ";
    Array1d<float>::print(query);

    Array1d<float> result = index.search(query);
    std::cout << "Result: ";
    Array1d<float>::print(result);

    return 0;
}
```