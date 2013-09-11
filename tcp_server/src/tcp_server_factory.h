/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "tcp_server_addon_impl.h"

namespace OpcUa
{
  namespace Impl
  {

    class TcpServerFactory : public Common::AddonFactory
    {
    public:
      Common::Addon::UniquePtr CreateAddon()
      {
        return Common::Addon::UniquePtr(new OpcUa::Impl::TcpServerAddon());
      }
    };

  }
}
