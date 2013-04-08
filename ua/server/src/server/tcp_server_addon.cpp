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

#include <iostream>

namespace
{

  class TcpServerAddon : public OpcUa::Server::TcpServerAddon
  {
  public:
    TcpServerAddon()
    {
      std::clog << "Created TcpServerAddon." << std::endl;
    }

    virtual ~TcpServerAddon()
    {
      std::clog << "Destroyed TcpServerAddon." << std::endl;
    }


    virtual void SetConnectionProcessor(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor)
    {
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& manager)
    {
      std::clog << "Initializing TcpServerAddon." << std::endl;
    }

    virtual void Stop()
    {
      std::clog << "Stopping TcpServerAddon." << std::endl;
    }

  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::TcpServerAddon());
}

