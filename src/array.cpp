
#include <cassert>
#include <iostream>

#include <pybind11/pybind11.h>

#include <array.hpp>

namespace py = pybind11;

#define EXPORT_ARRAY_2D(module, type) {                                                         \
    py::module m_sub = module.def_submodule(#type);                                             \
    py::class_<Array2d<type>>(m_sub, "Array2d", py::buffer_protocol())                          \
        .def_buffer([](Array2d<type> &a) -> py::buffer_info {                                   \
            return py::buffer_info(                                                             \
                *a.data,                               /* Pointer to buffer */                  \
                sizeof(type),                          /* Size of one scalar */                 \
                py::format_descriptor<type>::format(), /* Python struct-style /                 \
                                                        format descriptor */                    \
                2,                       /* Number of dimensions */                             \
                {a.rows, a.cols},        /* Buffer dimensions */                                \
                {sizeof(type) * a.cols, /* Strides (in bytes) for each index */                 \
                sizeof(type)});                                                                 \
        })                                                                                      \
        .def("allocate", &Array2d<type>::allocate)                                              \
        .def("free", &Array2d<type>::free)                                                      \
        .def("print", &Array2d<type>::print)                                                    \
        .def("from_numpy", [](py::buffer buffer, bool verbose = false) -> Array2d<type> {       \
            const py::buffer_info info = buffer.request();                                      \
                                                                                                \
            assert(py::format_descriptor<type>::format() == info.format);                       \
            assert(info.ndim == 2);                                                             \
                                                                                                \
            const unsigned int rows = info.shape[0];                                            \
            const unsigned int cols = info.shape[1];                                            \
            assert(info.strides[0] == cols * sizeof(type));                                     \
            assert(info.strides[1] == sizeof(type));                                            \
                                                                                                \
            type *data = (type*) info.ptr;                                                      \
            type **row_ptrs = new type*[rows];                                                  \
            for (unsigned int i = 0; i < rows; ++i)                                             \
                row_ptrs[i] = data + cols * i;                                                  \
                                                                                                \
            if (verbose) {                                                                      \
                std::cout << "Format: " << info.format << '\n';                                 \
                std::cout << "Shape: " << info.shape[0] << " x " << info.shape[1] << '\n';      \
                std::cout << "Strides: " << info.strides[0] << ", " << info.strides[1] << '\n'; \
                std::cout << "Size: " << info.size << '\n';                                     \
            }                                                                                   \
                                                                                                \
            return {row_ptrs, rows, cols};                                                      \
        }, py::arg("verbose") = false);                                                         \
}

PYBIND11_MODULE(banng, module) {
    EXPORT_ARRAY_2D(module, float);
    EXPORT_ARRAY_2D(module, double);
}
