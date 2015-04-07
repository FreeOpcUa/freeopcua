/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once

#include <boost/python.hpp>

#include "opc/ua/protocol/variant_visitor.h"

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
  object Result;

  template <typename T>
  void OnContainer(const T & val)
  {
    Result = ToList(val);
  }

  template <typename T>
  void OnScalar(const T & val)
  {
    Result = object(val);
  }
};

object ToObject(const Variant & var)
{
  if (var.IsNul())
    {
      return object();
    }

  VariantToPythonObjectConverter objectConverter;
  OpcUa::TypedVisitor<VariantToPythonObjectConverter> visitor(objectConverter);
  var.Visit(visitor);
  return objectConverter.Result;
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

          else if (extract<std::vector<NodeId>>(obj).check())
            {
              var = extract<std::vector<NodeId>>(obj)();
            }

          else if (extract<std::vector<DateTime>>(obj).check())
            {
              var = extract<std::vector<DateTime>>(obj)();
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

  else if (extract<NodeId>(obj).check())
    {
      var = extract<NodeId>(obj)();
    }
  else if (extract<DateTime>(obj).check())
    {
      var = extract<DateTime>(obj)();
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
    // Use borrowed to construct the object so that a reference
    // count will be properly handled.
    object obj = object(handle<>(borrowed(obj_ptr)));

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

