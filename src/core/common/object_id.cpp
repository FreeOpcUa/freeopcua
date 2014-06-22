/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief object id realizarion 
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/object_id.h>

#include <atomic>

Common::ObjectID Common::GenerateNewID()
{
  static std::atomic<Common::ObjectID> objectID;
  return ++objectID + 1000;
}

