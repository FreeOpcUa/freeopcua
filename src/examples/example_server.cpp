/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Server main.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///
#include <iostream>
#include <algorithm>
#include <time.h>

#include <thread>         
#include <chrono>   

#include <opc/ua/node.h>
#include <opc/ua/subscription.h>
#include <opc/ua/server/opcuaserver.h>
#include "src/server/daemon.h"




using namespace OpcUa;

class SubClient : public SubscriptionClient
{
  void DataChange(const Node& node, const Variant& val, AttributeID attr) override
  {
    std::cout << "Received DataChange event, value of Node " << node << " is now: "  << std::endl;
  }
};

int main(int argc, char** argv)
{
  OpcUa::OPCUAServer server;
  try
  {
    server.SetEndpoint("opc.tcp://localhost:4841");
    server.SetLoadCppAddressSpace(true);
    //server.AddAddressSpace("standard_address_space.xml");
    //server.AddAddressSpace("user_address_space.xml");
    server.Start();
    Node root = server.GetRootNode();
    std::cout << "Root node is: " << root << std::endl;
    std::cout << "Childs are: " << std::endl;
    for (Node node: root.GetChildren() )
    {
      std::cout << "    " << node << std::endl;
    }
    NodeID nid(99, 1);
    QualifiedName qn("NewObject", 2);
    root.AddObject(nid, qn);
   
    SubClient clt; 
    server.CreateSubscription(100, clt);

    std::cout << "Ctrl-C to exit" << std::endl;
    for(;;)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown error." << std::endl;
  }
  server.Stop();
  return 0;
}

