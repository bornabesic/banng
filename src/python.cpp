// BANNG (c) by Borna Bešić
// 
// BANNG is licensed under a
// Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// 
// You should have received a copy of the license along with this
// work. If not, see <http://creativecommons.org/licenses/by-nc-sa/4.0/>.

#include <cassert>
#include <iostream>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <array.hpp>
#include <memory.hpp>
#include <index.hpp>

namespace py = pybind11;

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
        .def("from_numpy", &numpy_to_array_2d<type>, py::arg("verbose") = false)                \
        .def("numpy", &array_2d_to_numpy<type>);                                                \
}

#define EXPORT_ARRAY_1D(module, type) {                                                         \
    py::module m_sub = module.def_submodule(#type);                                             \
    py::class_<Array1d<type>>(m_sub, "Array1d", py::buffer_protocol())                          \
        .def_buffer([](Array1d<type> &a) -> py::buffer_info {                                   \
            return py::buffer_info(                                                             \
                a.data,                                /* Pointer to buffer */                  \
                sizeof(type),                          /* Size of one scalar */                 \
                py::format_descriptor<type>::format(), /* Python struct-style /                 \
                                                       format descriptor */                     \
                1,                                     /* Number of dimensions */               \
                {a.length},                            /* Buffer dimensions */                  \
                {sizeof(type) * a.stride}              /* Strides (in bytes) for each index */  \
            );                                                                                  \
        })                                                                                      \
        .def("print", &Array1d<type>::print, py::arg("format") = "%.4f")                        \
        .def("from_numpy", &numpy_to_array_1d<type>, py::arg("verbose") = false)                \
        .def("numpy", &array_1d_to_numpy<type>);                                                \
}

#define EXPORT_INDEX(module, type, split) {                                                     \
    py::module m_sub = module.def_submodule(#type).def_submodule(#split);                       \
    py::class_<Index<type, split>>(m_sub, "Index")                                              \
        .def(py::init<>())                                                                      \
        .def("build", [](Index<type, split> &index, const py::buffer &buffer) -> void {         \
            auto array = numpy_to_array_2d<type>(buffer);                                       \
            index.build(array);                                                                 \
        })                                                                                      \
        .def("search", [](const Index<type, split> &index, const py::buffer &buffer)            \
            -> py::array_t<type, py::array::c_style | py::array::forcecast> {                   \
            auto array = numpy_to_array_1d<type>(buffer);                                       \
            auto result = index.search(array);                                                  \
            return array_1d_to_numpy(result);                                                   \
        });                                                                                     \
}

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

    return {
        (T *) info.ptr,
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

template <typename T>
py::array_t<T, py::array::c_style | py::array::forcecast> array_2d_to_numpy(const Array2d<T> &array) {
    return py::array_t<T, py::array::c_style | py::array::forcecast>(
        {array.rows, array.cols},
        {array.stride_rows * sizeof(T), array.stride_cols * sizeof(T)},
        *array.data
    );
}

template <typename T>
py::array_t<T, py::array::c_style | py::array::forcecast> array_1d_to_numpy(const Array1d<T> &array) {
    return py::array_t<T, py::array::c_style | py::array::forcecast>(
            {array.length},
            {array.stride * sizeof(T)},
            array.data
        );
}

PYBIND11_MODULE(banng, module) {
    EXPORT_ARRAY_2D(module, float);
    EXPORT_ARRAY_2D(module, double);

    EXPORT_ARRAY_1D(module, float);
    EXPORT_ARRAY_1D(module, double);

    EXPORT_INDEX(module, float, AxisAlignedSplit);
    EXPORT_INDEX(module, double, AxisAlignedSplit);
}
