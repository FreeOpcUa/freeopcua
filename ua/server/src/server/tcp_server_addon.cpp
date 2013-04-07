/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/tcp_server_addon.h>


namespace
{

  class TcpServerAddon : public OpcUa::Server::TcpServerAddon
  {
  public:
    virtual void SetConnectionProcessor(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor)
    {
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& manager)
    {
    }

    virtual void Stop()
    {
    }

  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::TcpServerAddon());
}

