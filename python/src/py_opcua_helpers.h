/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <vector>
#include <boost/python.hpp>

using namespace boost::python;

//
// vector<T> to python []
//

template<class T>
struct vector_to_python_converter
{

  static PyObject * convert(const std::vector<T> & vec)
  {
    list result;

    for (auto e : vec)
      { result.append(e); }

    return incref(result.ptr());
  }
};


//
// def_readwrite + conversion
//

#define def_readwrite_vector(NAME, FIELDREF)                                      \
add_property(NAME, make_getter(FIELDREF, return_value_policy<return_by_value>()), \
                   make_setter(FIELDREF, return_value_policy<return_by_value>()))

//
// python [] to vector<T>
//

template<typename T>
struct vector_from_python_converter
{

  vector_from_python_converter()
  {
    converter::registry::push_back(&convertible, &construct, type_id<std::vector<T>>());
  }

  static void * convertible(PyObject * obj_ptr)
  {
    if (PySequence_Check(obj_ptr))
      {
        for (Py_ssize_t i = 0; i < PySequence_Length(obj_ptr); i++)
          {
            if (!extract<T>(object(handle<>(PySequence_GetItem(obj_ptr, i)))).check())
              {
                return NULL;
              }
          }

        return obj_ptr;
      }

    else
      {
        return NULL;
      }
  }

  static void construct(PyObject * obj_ptr, converter::rvalue_from_python_stage1_data * data)
  {

    std::vector<T> vs;

    for (Py_ssize_t i = 0; i < PySequence_Length(obj_ptr); i++)
      {
        PyObject * elt = PySequence_GetItem(obj_ptr, i);
        vs.push_back(extract<T>(object(handle<>(borrowed(elt)))));
      }

    void * storage = ((converter::rvalue_from_python_storage<std::vector<T>> *)data)->storage.bytes;

    new(storage) std::vector<T>(vs);

    data->convertible = storage;

  }

};

