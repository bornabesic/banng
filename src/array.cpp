
#include <cassert>
#include <iostream>

#include <pybind11/pybind11.h>

#include <array.hpp>

namespace py = pybind11;

PYBIND11_MODULE(banng, m) {
    py::module m_float = m.def_submodule("float");
    py::class_<Array2d<float>>(m_float, "Array2d", py::buffer_protocol())
        .def_buffer([](Array2d<float> &a) -> py::buffer_info {
            return py::buffer_info(
                *a.data,                                /* Pointer to buffer */
                sizeof(float),                          /* Size of one scalar */
                py::format_descriptor<float>::format(), /* Python struct-style
                                                           format descriptor */
                2,                       /* Number of dimensions */
                {a.rows, a.cols},        /* Buffer dimensions */
                {sizeof(float) * a.cols, /* Strides (in bytes) for each index */
                 sizeof(float)});
        })
        .def("allocate", &Array2d<float>::allocate)
        .def("free", &Array2d<float>::free)
        .def("print", &Array2d<float>::print)
        .def("from_numpy", [](py::buffer buffer, bool verbose = false) -> Array2d<float> {
            const py::buffer_info info = buffer.request();

            assert(py::format_descriptor<float>::format() == info.format);
            assert(info.ndim == 2);

            const unsigned int rows = info.shape[0];
            const unsigned int cols = info.shape[1];
            assert(info.strides[0] == cols * sizeof(float));
            assert(info.strides[1] == sizeof(float));

            float *data = (float*) info.ptr;
            float **row_ptrs = new float*[rows];
            for (unsigned int i = 0; i < rows; ++i)
                row_ptrs[i] = data + cols * i;

            if (verbose) {
                std::cout << "Format: " << info.format << '\n';
                std::cout << "Shape: " << info.shape[0] << " x " << info.shape[1] << '\n';
                std::cout << "Strides: " << info.strides[0] << ", " << info.strides[1] << '\n';
                std::cout << "Size: " << info.size << '\n';
            }

            return {row_ptrs, rows, cols};
        }, py::arg("verbose") = false);
}