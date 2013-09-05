/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <boost/python.hpp>

char const* greet()
{
   return "hello, world";
}


BOOST_PYTHON_MODULE(opcua)
{
  using namespace boost::python;
  def("greet", greet);
}
