#include <iostream>
#include "bind.hh"
#include <pybind11/pybind11.h>
namespace py = pybind11;

PYBIND11_MODULE(ex10,m){
	py::class_<bind>(m,"bind").def(py::init<>()).def("createThreads",&bind::createThreads,"Setup function. Creates producer and consumer threads. Input: (int set_tmax,int maxiter,int numConsumerThreads).").def("fillQ",&bind::fillQ,"Fills the queue").def("produce",&bind::produce,"produces").def("consume",&bind::consume,"consumes");
	m.doc() = "threading plugin";
}
