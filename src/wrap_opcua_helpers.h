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

struct string_vector_from_python_converter
{
  string_vector_from_python_converter();
  static void * convertible(PyObject *);
  static void construct(PyObject *, converter::rvalue_from_python_stage1_data *);
};

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

#endif


