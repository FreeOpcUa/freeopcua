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

Variant ToVariant(const object & obj)
{
  Variant var;

  if (extract<std::string>(obj).check())
    {
      var = extract<std::string>(obj)();
    }

  else if (extract<list>(obj).check())
    {

      if (len(obj) > 0)
        {
          if (extract<long>(obj[0]).check())
            {
              var = ToVector<long>(obj);
            }

          else if (extract<double>(obj[0]).check())
            {
              var = ToVector<double>(obj);
            }

          else if (extract<std::vector<std::string>>(obj).check())
            {
              var = extract<std::vector<std::string>>(obj)();
            }

          else if (extract<std::vector<NodeID>>(obj).check())
            {
              var = extract<std::vector<NodeID>>(obj)();
            }

          else
            {
              throw std::logic_error("Cannot create variant from python list. Unsupported type.");
            }
        }
    }

  else if (extract<long>(obj).check())
    {
      var = extract<long>(obj)();
    }

  else if (extract<double>(obj).check())
    {
      var = extract<double>(obj)();
    }

  else if (extract<NodeID>(obj).check())
    {
      var = extract<NodeID>(obj)();
    }

  else
    {
      throw std::logic_error("Cannot create variant from python object. Unsupported type.");
    }

  return var;
}


//similar to ToVariant but gives a hint to what c++ object type the python object should be converted to
Variant ToVariant2(const object & obj, VariantType vtype)
{
  Variant var;

  if (extract<list>(obj).check())
    {

      if (len(obj) == 0)
        {
          return var;
        }

      else
        {
          switch (vtype)
            {
            case VariantType::BOOLEAN:
              var = ToVector<bool>(obj);
              return var;

            case VariantType::UINT16:
            case VariantType::UINT32:
              var = ToVector<uint32_t>(obj);
              return var;

            case VariantType::FLOAT:
              var = ToVector<float>(obj);
              return var;

            default:
              return ToVariant(obj);
            }
        }
    }

  else
    {
      switch (vtype)
        {
        case VariantType::BOOLEAN:
          var = extract<bool>(obj)();
          return var;

        case VariantType::UINT16:
        case VariantType::UINT32:
          var = extract<uint32_t>(obj)();
          return var;

        case VariantType::FLOAT:
          var = extract<float>(obj)();
          return var;

        default:
          return ToVariant(obj);
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

    void * storage = ((converter::rvalue_from_python_storage<Variant> *)data)->storage.bytes;

    new(storage) Variant(ToVariant(obj));

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

