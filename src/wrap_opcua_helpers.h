/// @author Matthieu Bec 2014
/// @email mdcb808@gmail.com
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef WRAP_OPCUA_HELPERS_H
#define WRAP_OPCUA_HELPERS_H

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


template<class T, class U>
struct vector_property_adapter
{

  template<std::vector<U> T::* V>
  static void setter(T & self, std::vector<U> v)
  {
    self.*V = v;
  }

  template<std::vector<U> T::* V>
  static std::vector<U> getter(const T & self)
  {
    return self.*V;
  }

//  static auto make_adapter(std::vector<U> T::* V) -> decltype(&vector_property_adapter<T, U>)
//  {
//    return &vector_property_adapter<T, U>;
//  }

};


// XXX ugly
#define  add_vector_property(NAME, CLASS, TYPE, FIELD)                          \
add_property(NAME, &vector_property_adapter<CLASS,TYPE>::getter<&CLASS::FIELD>, \
                   &vector_property_adapter<CLASS,TYPE>::setter<&CLASS::FIELD>)


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
        vs.push_back(extract<T>(object(handle<>(elt))));
      }

    void * storage = ((converter::rvalue_from_python_storage<std::vector<T>> *)data)->storage.bytes;

    new(storage) std::vector<T>(vs);

    data->convertible = storage;

  }

};


#endif


