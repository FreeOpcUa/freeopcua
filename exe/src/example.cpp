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

#include <opc/common/application.h>
#include <opc/common/addons_core/config_file.h>
#include <opc/ua/node.h>

#include <opc/ua/opcuaserver.h>
#include "daemon.h"




using namespace OpcUa;

int main(int argc, char** argv)
{
  OpcUa::OPCUAServer server;
  try
  {
    server.AddAddressSpace("standard_address_space.xml");
    //server.AddAddressSpace("user_address_space.xml");
    server.Start();
    Node root = server.GetRootNode();
    std::cout << "Root node is: " << root << std::endl;
    std::cout << "Childs are: " << std::endl;
    for (Node node: root.GetChildren() )
    {
      std::cout << "    " << node << std::endl;
    }

    
  /*

    OpcUa::WriteValue attribute;
    NodeID nodeid(ObjectID::NamespaceArray);
    //NodeID nodeid = OpcUa::NumericNodeID(88);
    //nodeid.NumericNodeID(ObjectID::NamespaceArray, 0);
    attribute.Node = nodeid;
    attribute.Attribute = AttributeID::VALUE;
    std::vector<std::string> val;
    val.push_back("http://opcfoundation.org/UA/");
    val.push_back("http://sintef.no/Devices/");
    val.push_back("http://inted");
    //int a[] = {0, 2, 3};
    //std::string a[] = {"http://mytest", "http://sintef.no", "https://lkjsdflkjsd"};
    OpcUa::Variant value(val);
    //value.Value = a;
    //value.IsArray
    attribute.Data = val;
    std::cout << "variant is: " << value.IsArray()   << std::endl;
    //std::cout << "variant is: " << value.IsArray() << value.Type << std::endl;
    std::vector<StatusCode> statuses = registry->GetServer()->Attributes()->Write(std::vector<OpcUa::WriteValue>(1, attribute));
    //std::vector<StatusCode> statuses = registry.Write(std::vector<OpcUa::WriteValue>(1, attribute));
    for (OpcUa::StatusCode status : statuses)
    {
      std::cout << "Status code: 0x" << std::hex << static_cast<uint32_t>(status) << std::endl;
    }

    Node objects(registry->GetServer(), OpcUa::ObjectID::ObjectsFolder);
    std::cout << "node is: " << objects << std::endl;

    std::cout << "browsing object folder: " << std::endl;
    for (Node n: objects.Browse())
    {
        std::cout << "    " << n << std::endl;
    }
    */
    sleep(1);
    //server.Stop();
//OpcUa::Daemon daemon;

//daemon.WaitForTerminate();
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

