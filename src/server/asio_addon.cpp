/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include <opc/ua/server/addons/asio_addon.h>

#include <boost/asio.hpp>

#include <algorithm>
#include <iostream>
#include <thread>

namespace
{
class AsioAddonImpl : public OpcUa::Server::AsioAddon
{
public:
  AsioAddonImpl()
    : Work(IoService)
  {
  }


  void Initialize(Common::AddonsManager &, const Common::AddonParameters & params) override
  {
    const unsigned threadsNumber = GetThreadsNumber(params);

    //std::cout << "asio| Starting " << threadsNumber << "threads." << std::endl;
    for (unsigned i = 0; i < threadsNumber; ++i)
      {
        Threads.emplace_back([this, i]()
        {
          //std::cout << "asio| Starting thread " << i << "." << std::endl;
          IoService.run();
          //std::cout << "asio| Thread " << i << "exited." << std::endl;
        });
      }
  }

  void Stop() override
  {
    //std::cout << "asio| stopping io service." << std::endl;
    IoService.stop();
    //std::cout << "asio| joining threads." << std::endl;
    std::for_each(Threads.begin(), Threads.end(), [](std::thread & thread)
    {
      thread.join();
    });
  }

  virtual boost::asio::io_service & GetIoService() override
  {
    return IoService;
  }

  unsigned GetThreadsNumber(const Common::AddonParameters & params) const
  {
    unsigned num = 1;

    for (auto paramIt : params.Parameters)
      {
        if (paramIt.Name == "threads")
          {
            num = std::stoi(paramIt.Value);
            break;
          }
      }

    return num;
  }

private:
  boost::asio::io_service IoService;
  boost::asio::io_service::work Work;
  std::vector<std::thread> Threads;
};
}


namespace OpcUa
{
namespace Server
{

Common::Addon::UniquePtr AsioAddonFactory::CreateAddon()
{
  return Common::Addon::UniquePtr(new AsioAddonImpl);
}

}
}
