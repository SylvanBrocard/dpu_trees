#include <pybind11/cast.h>
#include <pybind11/detail/common.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/pytypes.h>
#include <iostream>


extern "C"
{
#include <dpu.h>
#include "trees.h"
#include "trees_common.h"
}

using namespace pybind11::literals; /* necessary for the '_a' keyword in py::dict definition */

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int add(int i, int j)
{
    return i + j;
}

extern "C" int checksum(char *);

namespace py = pybind11;

py::object random_splitter_class = py::module::import("sklearn.tree._splitter").attr("RandomSplitter");
py::object parent_metaclass = py::reinterpret_borrow<py::object>((PyObject *) &PyType_Type);
py::dict attributes = py::dict(
    "test"_a = py::cpp_function(
        [](py::object self, py::object x) {
            py::print(x);
        },
        py::is_method(py::none())
    ),
    "node_reset"_a = py::cpp_function(
        [](py::object self, size_t start, size_t end, double* weighted_n_node_samples) {
        },
        py::is_method(py::none())
    ),
    "node_split"_a = py::cpp_function(
        [](py::object self, double impurity, py::object* split, size_t* n_constant_features) {
        },
        py::is_method(py::none())
    )
);

/**
 * @brief Container class for interfacing with python
 *
 * This class holds data that can be reused
 * during different runs of the k-means algorithm.
 */
class Container
{
private:
    Params p;
    feature_t **features_float;
    int *targets;

public:
    /**
     * @brief Allocates all DPUs.
     */
    void allocate()
    {
        ::allocate(&p);
    }

    /**
     * @brief Loads binary into the DPUs
     *
     * @param DPU_BINARY Path to the binary.
     */
    void load_kernel(const char *DPU_BINARY)
    {
        ::load_kernel(&p, DPU_BINARY);
    }
    // /**
    //  * @brief Loads data into the DPU from a file.
    //  *
    //  * @param filename Path to the data file.
    //  * @param is_binary_file Whether the data is encoded as binary.
    //  */
    // void load_file_data(const char *filename, bool is_binary_file)
    // {
    //     p.from_file = true;
    //     if (is_binary_file)
    //         read_bin_input_with_int_targets(&p, filename, &features_float, &targets);
    //     else
    //     {
    //         read_txt_input_with_int_target(&p, filename, &features_float, &targets);
    //         save_dat_file_with_int_target(&p, filename, features_float, targets);
    //     }
    //     transfer_data();
    // }
    /**
     * @brief Loads data into the DPUs from a python array
     *
     * @param data A python ndarray.
     * @param npoints Number of points.
     * @param nfeatures Number of features.
     * @param threshold Parameter to declare convergence.
     * @param verbose Verbosity level.
     */
    void load_array_data(py::array_t<float> data, py::array_t<float> features_mins, py::array_t<float> features_maxes, uint64_t npoints, int nfeatures)
    {
        float *data_ptr = (float *)data.request().ptr;
        float *mins_ptr = (float *)features_mins.request().ptr;
        float *maxes_ptr = (float *)features_maxes.request().ptr;

        p.from_file = false;

        p.npoints = npoints;
        p.nfeatures = nfeatures;
        format_array_input(&p, data_ptr, &features_float);
        transfer_data();
    }
    /**
     * @brief Preprocesses and transfers data to the DPUs
     *
     */
    void transfer_data()
    {
        // preprocessing(&p, features_float, &features_int, verbose);
        populateDpu(&p, features_float);
        // allocateMemory(&p);
        // #ifdef FLT_REDUCE
        // allocateMembershipTable(&p);
        // #endif
    }
    /**
     * @brief Frees the data.
     */
    void free_data(bool from_file, bool restore_features)
    {
        /* We are NOT freeing the underlying float array if it is managed by python */
        if (from_file)
            free(features_float[0]);
        // else if (restore_features)
        //     postprocessing(&p, features_float);
        free(features_float);
        // free(p.mean);
        #ifdef FLT_REDUCE
        deallocateMembershipTable();
        #endif
    }
    /**
     * @brief Frees the DPUs
     */
    void free_dpus()
    {
        ::free_dpus(&p);
        // deallocateMemory();
    }

    /**
    * @brief Main function for the KMeans algorithm.
    *
    * @return py::array_t<float> The centroids coordinates found by the algorithm.
    // */
    // py::array_t<float> trees_cpp(
    //     int isOutput,                    /**< whether or not to print the centroids */
    //     int nloops,                      /**< how many times the algorithm will be executed for each number of clusters */
    //     int max_iter,                    /**< upper bound of the number of iterations */
    //     py::array_t<int> log_iterations, /**< array logging the iterations per nclusters */
    //     py::array_t<double> log_time)    /**< array logging the time taken per nclusters */
    // {
    //     int *log_iter_ptr = (int *)log_iterations.request().ptr;
    //     double *log_time_ptr = (double *)log_time.request().ptr;

    //     float *clusters = kmeans_c(
    //         &p,
    //         features_float,
    //         features_int,
    //         log_iter_ptr,
    //         log_time_ptr);

    //     std::vector<ssize_t> shape = {best_nclusters, p.nfeatures};
    //     std::vector<ssize_t> strides = {(int)sizeof(float) * p.nfeatures, sizeof(float)};

    //     py::capsule free_when_done(clusters, [](void *f)
    //                             { delete reinterpret_cast<float *>(f); });

    //     return py::array_t<float>(
    //         shape,
    //         strides,
    //         clusters,
    //         free_when_done);
    // }
};

PYBIND11_MODULE(_core, m)
{
    m.doc() = R"pbdoc(
        DPU kmeans plugin
        -----------------

        .. currentmodule:: dpu_kmeans

        .. autosummary::
           :toctree: _generate

           add
           subtract
           call_home
           dpu_test
           checksum
           kmeans_cpp
    )pbdoc";

    py::class_<Container>(m, "Container", R"pbdoc(
        Container object to interface with the DPUs
    )pbdoc")
        .def(py::init<>())
        .def("allocate", &Container::allocate)
        .def("load_kernel", &Container::load_kernel)
        .def("load_array_data", &Container::load_array_data)
        .def("free_data", &Container::free_data)
        .def("free_dpus", &Container::free_dpus);
        // .def("kmeans", &Container::trees_cpp);

    m.def("add", &add, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");

    m.def(
        "subtract", [](int i, int j)
        { return i - j; },
        R"pbdoc(
        Subtract two numbers

        Some other explanation about the subtract function.
    )pbdoc");

    m.def("checksum", &checksum, R"pbdoc(
        Checksum test on dpus
    )pbdoc");

    m.attr("RandomDPUSplitter") = parent_metaclass("RandomDPUSplitter", py::make_tuple(random_splitter_class), attributes);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
