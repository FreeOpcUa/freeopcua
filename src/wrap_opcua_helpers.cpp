/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "wrap_opcua_helpers.h"


string_vector_from_python_converter::string_vector_from_python_converter()
{
  converter::registry::push_back(&string_vector_from_python_converter::convertible, &string_vector_from_python_converter::construct, type_id<std::vector<std::string>>());
}

void * string_vector_from_python_converter::convertible(PyObject * obj_ptr)
{
  if (PySequence_Check(obj_ptr))
    {
      for (Py_ssize_t i = 0; i < PySequence_Length(obj_ptr); i++)
        {
          if (!PyString_Check(PySequence_GetItem(obj_ptr, i)))
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

void string_vector_from_python_converter::construct(PyObject * obj_ptr, converter::rvalue_from_python_stage1_data * data)
{

  std::vector<std::string> vs;

  for (Py_ssize_t i = 0; i < PySequence_Length(obj_ptr); i++)
    {
      PyObject * elt = PySequence_GetItem(obj_ptr, i);
      vs.push_back(PyString_AsString(elt));
    }

  void * storage = ((converter::rvalue_from_python_storage<std::vector<std::string>> *)data)->storage.bytes;

  new(storage) std::vector<std::string>(vs);

  data->convertible = storage;

}


