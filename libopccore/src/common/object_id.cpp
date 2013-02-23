/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief object id realizarion 
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opccore/common/object_id.h>

Common::ObjectID Common::GenerateNewID()
{
  // TODO thread safe
  static Common::ObjectID objectID = 0;
  return ++objectID;
}

