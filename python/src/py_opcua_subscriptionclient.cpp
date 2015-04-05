/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "py_opcua_subscriptionclient.h"

static std::string parse_python_exception()
{
  PyObject * type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
  // Fetch the exception info from the Python C API
  PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
  // Fallback error
  std::string ret("Unfetchable Python error");

  // If the fetch got a type pointer, parse the type into the exception string
  if (type_ptr != NULL)
    {
      handle<> h_type(type_ptr);
      str type_pstr(h_type);
      // Extract the string from the boost::python object
      extract<std::string> e_type_pstr(type_pstr);

      // If a valid string extraction is available, use it
      // otherwise use fallback
      if (e_type_pstr.check())
        { ret = e_type_pstr(); }

      else
        { ret = "Unknown exception type"; }
    }

  // Do the same for the exception value (the stringification of the exception)
  if (value_ptr != NULL)
    {
      handle<> h_val(value_ptr);
      str a(h_val);
      extract<std::string> returned(a);

      if (returned.check())
        { ret += ": " + returned(); }

      else
        { ret += std::string(": Unparseable Python error: "); }
    }

  // Parse lines from the traceback using the Python traceback module
  if (traceback_ptr != NULL)
    {
      handle<> h_tb(traceback_ptr);
      // Load the traceback module and the format_tb function
      object tb(import("traceback"));
      object fmt_tb(tb.attr("format_tb"));
      // Call format_tb to get a list of traceback strings
      object tb_list(fmt_tb(h_tb));
      // Join the traceback strings into a single string
      object tb_str(str("\n").join(tb_list));
      // Extract the string, check the extraction, and fallback in necessary
      extract<std::string> returned(tb_str);

      if (returned.check())
        { ret += ": " + returned(); }

      else
        { ret += std::string(": Unparseable Python traceback"); }
    }

  return ret;
}

PySubscriptionHandler::PySubscriptionHandler(PyObject * p)
  : self(p)
{}

void PySubscriptionHandler::DataChange(uint32_t handle, const Node & node, const Variant & val, AttributeId attribute) const
{
  PyGILState_STATE state = PyGILState_Ensure();

  try
    {
      call_method<void>(self, "data_change", handle, node, val , attribute);
    }

  catch (const error_already_set & ex)
    {
      std::string perror_str = parse_python_exception();
      std::cout << "Error in 'data_change' method handler: " << perror_str << std::endl;
    }

  PyGILState_Release(state);
}

void PySubscriptionHandler::Event(uint32_t handle, const OpcUa::Event & event) const
{
  PyGILState_STATE state = PyGILState_Ensure();

  try
    {
      call_method<void>(self, "event", handle, event);
    }
  catch (const error_already_set & ex)
    {
      std::string perror_str = parse_python_exception();
      std::cout << "Error in 'event' method handler: " << perror_str << std::endl;
    }

  PyGILState_Release(state);
}

void PySubscriptionHandler::StatusChange(StatusCode status) const
{
  PyGILState_STATE state = PyGILState_Ensure();

  try
    {
      call_method<void>(self, "status_change", status);
    }

  catch (const error_already_set & ex)
    {
      std::string perror_str = parse_python_exception();
      std::cout << "Error in 'status_change' method handler: " << perror_str << std::endl;
    }

  PyGILState_Release(state);
}

void PySubscriptionHandler::DefaultDataChange(const SubscriptionHandler & self_, uint32_t handle, const Node & node, const object & val, uint32_t attribute)
{
  std::cout << "'data_change' virtual in this context" << std::endl;
}

void PySubscriptionHandler::DefaultEvent(const SubscriptionHandler & self_, uint32_t handle, const OpcUa::Event & event)
{
  std::cout << "'event' virtual in this context" << std::endl;
}

void PySubscriptionHandler::DefaultStatusChange(const SubscriptionHandler & self_, StatusCode status)
{
  std::cout << "'status_change' virtual in this context" << std::endl;
}


