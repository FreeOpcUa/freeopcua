/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Create Io Manager object
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __manaher_impl_h_277b8d9e_3724_4967_a65b_b68eb16e6265
#define __manaher_impl_h_277b8d9e_3724_4967_a65b_b68eb16e6265

#include <memory>

namespace InputOutputManager
{
  class RequestManager;
  std::unique_ptr<RequestManager> CreateRequestManager();
}

#endif // __manaher_impl_h_277b8d9e_3724_4967_a65b_b68eb16e6265
