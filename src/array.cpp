
#include <pybind11/pybind11.h>

#include <array.hpp>

namespace py = pybind11;

PYBIND11_MODULE(banng, m) {
    py::class_<Array2d<float>>(m, "Array2d", py::buffer_protocol())
        .def_buffer([](Array2d<float> &a) -> py::buffer_info {
            return py::buffer_info(
                a.data,                                 /* Pointer to buffer */
                sizeof(float),                          /* Size of one scalar */
                py::format_descriptor<float>::format(), /* Python struct-style
                                                        format descriptor */
                2,                       /* Number of dimensions */
                {a.rows, a.cols},        /* Buffer dimensions */
                {sizeof(float) * a.cols, /* Strides (in bytes) for each index */
                 sizeof(float)});
        })
        .def("allocate", &Array2d<float>::allocate)
        .def("free", &Array2d<float>::free);
}