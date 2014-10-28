/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef WRAP_OPCUA_VARIANTS_H
#define WRAP_OPCUA_VARIANTS_H

#include <boost/python.hpp>

using namespace boost::python;

template <typename T>
list ToList(const std::vector<T> objects)
{
  list result;

  for (auto obj : objects)
    {
      result.append(obj);
    }

  return result;
}

template <typename ResultType, typename SourceType>
list ToList(const std::vector<SourceType> objects)
{
  list result;

  for (auto obj : objects)
    {
      result.append(ResultType(obj));
    }

  return result;
}

template <typename T>
std::vector<T> ToVector(const object & list)
{
  std::vector<T> result;
  std::size_t listSize = len(list);

  for (std::size_t i = 0; i < listSize; ++i)
    {
      const object & element = list[i];
      const T & value = extract<T>(element)();
      result.push_back(value);
    }

  return result;
}

struct VariantToPythonObjectConverter
{
  typedef object result_type;

  template <typename T>
  typename std::enable_if<is_container_not_string<T>::value == true, result_type>::type operator()(const T & val)
  {
    return ToList(val);
  }

  template <typename T>
  typename std::enable_if<is_container_not_string<T>::value == false, result_type>::type operator()(const T & val)
  {
    return object(val);
  }
};

object ToObject(const Variant & var)
{
  if (var.IsNul())
    {
      return object();
    }

  return var.Visit(VariantToPythonObjectConverter());
}

Variant ToVariant(const object & object)
{
  Variant var;

  if (extract<std::string>(object).check())
    {
      var = extract<std::string>(object)();
    }

  else if (extract<list>(object).check())
    {

      if (len(object) > 0)
        {
          if (extract<int>(object[0]).check())
            {
              var = ToVector<int>(object);
            }

          else if (extract<double>(object[0]).check())
            {
              var = ToVector<double>(object);
            }

          else if (extract<std::vector<std::string>>(object).check())
            {
              var = extract<std::vector<std::string>>(object)();
            }

          else if (extract<std::vector<NodeID>>(object).check())
            {
              var = extract<std::vector<NodeID>>(object)();
            }

          else
            {
              throw std::logic_error("Cannot create variant from python list. Unsupported type.");
            }
        }
    }

  else if (extract<int>(object).check())
    {
      var = extract<int>(object)();
    }

  else if (extract<double>(object).check())
    {
      var = extract<double>(object)();
    }

  else if (extract<NodeID>(object).check())
    {
      var = extract<NodeID>(object)();
    }

  else
    {
      throw std::logic_error("Cannot create variant from python object. Unsupported type.");
    }

  return var;
}


//similar to ToVariant but gives a hint to what c++ object type the python object should be converted to
Variant ToVariant2(const object & object, VariantType vtype)
{
  Variant var;

  if (extract<list>(object).check())
    {

      if (len(object) == 0)
        {
          return var;
        }

      else
        {
          switch (vtype)
            {
            case VariantType::BOOLEAN:
              var = ToVector<bool>(object);
              return var;

            case VariantType::UINT16:
            case VariantType::UINT32:
              var = ToVector<uint32_t>(object);
              return var;
            
            case VariantType::FLOAT:
              var = ToVector<float>(object);
              return var;

            default:
              return ToVariant(object);
            }
        }
    }

  else
    {
      switch (vtype)
        {
        case VariantType::BOOLEAN:
          var = extract<bool>(object)();
          return var;

        case VariantType::UINT16:
        case VariantType::UINT32:
          var = extract<uint32_t>(object)();
          return var;

        case VariantType::FLOAT:
          var = extract<float>(object)();
          return var;

        default:
          return ToVariant(object);
        }
    }
}


struct variant_from_python_converter
{
  variant_from_python_converter()
  {
    converter::registry::push_back(&convertible, &construct, type_id<Variant>());
  }

  static void * convertible(PyObject * obj_ptr)
  {
    return obj_ptr; // TODO XXX
  }

  static void construct(PyObject * obj_ptr, converter::rvalue_from_python_stage1_data * data)
  {

    object obj = object(handle<>(obj_ptr));

    Variant var = ToVariant(obj);

    void * storage = ((converter::rvalue_from_python_storage<Variant> *)data)->storage.bytes;

    new(storage) Variant(var);

    data->convertible = storage;

  }
};


struct variant_to_python_converter
{
  static PyObject * convert(Variant const & v)
  {

    if (v.IsNul())
      {
        Py_INCREF(Py_None);
        return Py_None;
      }

    object obj = ToObject(v);
    return incref(obj.ptr());
  }
};






#endif

