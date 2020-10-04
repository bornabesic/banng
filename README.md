# BANNG 💥
Borna's Approximate Nearest Neighbor Gearbox

# Building
```sh
mkdir build
cd build
cmake .. -D CMAKE_BUILD_TYPE=Release
```

# Example
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