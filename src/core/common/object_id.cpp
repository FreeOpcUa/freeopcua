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

Common::ObjectId Common::GenerateNewId()
{
  static std::atomic<Common::ObjectId> objectId;
  return ++objectId + 1000;
}

