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
  void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeID attr) const override
  {
    std::cout << "Received DataChange event for Node " << node << std::endl;
  }
};

int main(int argc, char** argv)
{
  OpcUa::OPCUAServer server(true);
  //try
  //{
    server.SetEndpoint("opc.tcp://localhost:4841");
    server.SetLoadCppAddressSpace(true);
    server.Start();
    Node root = server.GetRootNode();
    std::cout << "Root node is: " << root << std::endl;
    std::cout << "Childs are: " << std::endl;
    for (Node node: root.GetChildren() )
    {
      std::cout << "    " << node << std::endl;
    }

    Node objects = server.GetObjectsNode();

    //create a new object in addressspace
    NodeID nid(99, 1);
    QualifiedName qn("NewObject", 2);
    Node newobject = objects.AddObject(nid, qn);
    //Add a virable to objevt
    Node myvar = newobject.AddVariable(NodeID(999, 0), QualifiedName("MyVariable", 2), Variant(8));
   
    SubClient clt; 
    std::unique_ptr<Subscription> sub = server.CreateSubscription(100, clt);
    sub->SubscribeDataChange(myvar);
    uint32_t counter = 0;
    myvar.SetValue(Variant(counter)); //will change value and trigger datachange event
    std::cout << "Ctrl-C to exit" << std::endl;
    for(;;)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      myvar.SetValue(Variant(++counter)); //will change value and trigger datachange event
    }

  //}
  //catch (const std::exception& exc)
  //{
    //std::cout << "Exception: " << exc.what() << std::endl;
    //throw(exc);
  //}
  //catch (...)
  //{
    //std::cout << "Unknown error." << std::endl;
  //}
  server.Stop();
  return 0;
}

