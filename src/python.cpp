
#include <cassert>
#include <iostream>

#include <pybind11/pybind11.h>

#include <array.hpp>
#include <memory.hpp>
#include <index.hpp>

namespace py = pybind11;

template <typename T>
Array1d<T> numpy_to_array_1d(const py::buffer &buffer, bool verbose = false) {
    const py::buffer_info info = buffer.request();

    assert(py::format_descriptor<T>::format() == info.format);
    assert(info.ndim == 1);

    const unsigned int length = info.shape[0];

    if (verbose) {
        std::cout << "Format: " << info.format << '\n';
        std::cout << "Shape: " << info.shape[0] << '\n';
        std::cout << "Strides: " << info.strides[0] << '\n';
        std::cout << "Size: " << info.size << '\n';
    }

    return { // TODO Make Array1d compatible with Memory
        std::make_shared<Memory<T>>((T *)info.ptr, info.size),
        length,
        info.strides[0] / sizeof(T)
    };
}

template <typename T>
Array2d<T> numpy_to_array_2d(const py::buffer &buffer, bool verbose = false) {
    const py::buffer_info info = buffer.request();

    assert(py::format_descriptor<T>::format() == info.format);
    assert(info.ndim == 2);

    const unsigned int rows = info.shape[0];
    const unsigned int cols = info.shape[1];

    if (verbose) {
        std::cout << "Format: " << info.format << '\n';
        std::cout << "Shape: " << info.shape[0] << " x " << info.shape[1] << '\n';
        std::cout << "Strides: " << info.strides[0] << ", " << info.strides[1] << '\n';
        std::cout << "Size: " << info.size << '\n';
    }

    return {
        std::make_shared<Memory<T>>((T *)info.ptr, info.size),
        rows, cols,
        info.strides[0] / sizeof(T), info.strides[1] / sizeof(T)
    };
}

#define EXPORT_ARRAY_2D(module, type) {                                                         \
    py::module m_sub = module.def_submodule(#type);                                             \
    py::class_<Array2d<type>>(m_sub, "Array2d", py::buffer_protocol())                          \
        .def_buffer([](Array2d<type> &a) -> py::buffer_info {                                   \
            return py::buffer_info(                                                             \
                *a.data,                               /* Pointer to buffer */                  \
                sizeof(type),                          /* Size of one scalar */                 \
                py::format_descriptor<type>::format(), /* Python struct-style /                 \
                                                       format descriptor */                     \
                2,                                     /* Number of dimensions */               \
                {a.rows, a.cols},                      /* Buffer dimensions */                  \
                {sizeof(type) * a.stride_rows,         /* Strides (in bytes) for each index */  \
                sizeof(type) * a.stride_cols}                                                   \
            );                                                                                  \
        })                                                                                      \
        .def("allocate", (Array2d<type> (*)(unsigned int, unsigned int))                        \
                         &Array2d<type>::allocate)                                              \
        .def("allocate", (Array2d<type> (*)(unsigned int, unsigned int, const std::string &))   \
                         &Array2d<type>::allocate)                                              \
        .def("print", &Array2d<type>::print, py::arg("format") = "%.4f")                        \
        .def("from_numpy", &numpy_to_array_2d<type>, py::arg("verbose") = false);               \
}

PYBIND11_MODULE(banng, module) {
    EXPORT_ARRAY_2D(module, float);
    EXPORT_ARRAY_2D(module, double);

    // TODO Make generic
    py::class_<Index<float, AxisAlignedSplit>>(module, "Index")
    .def(py::init<>())
    .def("build", [](Index<float, AxisAlignedSplit> &index, const py::buffer &buffer) -> void {
        auto array = numpy_to_array_2d<float>(buffer);
        index.build(array);
    })
    .def("search", [](const Index<float, AxisAlignedSplit> &index, const py::buffer &buffer) -> Array1d<float> {
        auto array = numpy_to_array_1d<float>(buffer);
        return index.search(array);
    });
}
