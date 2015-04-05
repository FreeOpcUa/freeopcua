/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///



#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>

#include <iostream>
#include <stdexcept>
#include <thread>

using namespace OpcUa;

class SubClient : public SubscriptionHandler
{
  void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) const override
  {
    std::cout << "Received DataChange event, value of Node " << node << " is now: "  << "(Print of arbitrary variant not suported yet)"<< std::endl;
  }
};

int main(int argc, char** argv)
{
  try
  {
    //std::string endpoint = "opc.tcp://192.168.56.101:48030";
    //std::string endpoint = "opc.tcp://user:password@192.168.56.101:48030";
    std::string endpoint = "opc.tcp://127.0.0.1:4841/freeopcua/server/";
    //std::string endpoint = "opc.tcp://localhost:53530/OPCUA/SimulationServer/";

    if(argc > 1)
      endpoint = argv[1];

    std::cout << "Connecting to: " << endpoint << std::endl;
    bool debug = false;
    OpcUa::UaClient client(debug);
    client.Connect(endpoint);

    //get Root node on server
    OpcUa::Node root = client.GetRootNode();
    std::cout << "Root node is: " << root << std::endl;

    //get and browse Objects node
    std::cout << "Child of objects node are: " << std::endl;
    Node objects = client.GetObjectsNode();
    for (OpcUa::Node node : objects.GetChildren())
      std::cout << "    " << node << std::endl;

    //get a node from standard namespace using objectId
    std::cout << "NamespaceArray is: " << std::endl;
    OpcUa::Node nsnode = client.GetNode(ObjectId::Server_NamespaceArray); 
    OpcUa::Variant ns  = nsnode.GetValue();

    for (std::string d : ns.As<std::vector<std::string>>())
      std::cout << "    "  << d << std::endl;
    
    //Get namespace index we are interrested in
    uint32_t idx = client.GetNamespaceIndex("http://examples.freeopcua.github.io");
    
    //Get Node using path (BrowsePathToNodeId call)
    std::vector<std::string> varpath({std::to_string(idx) + ":NewObject", "MyVariable"});
    //std::vector<std::string> varpath({"Objects", "5:Simulation", "5:Random1"}); //Example data from prosys server
    OpcUa::Node myvar = objects.GetChild(varpath);
    std::cout << "got node: " << myvar << std::endl;

    //Subscription
    SubClient sclt; 
    std::unique_ptr<Subscription> sub = client.CreateSubscription(100, sclt);
    uint32_t handle = sub->SubscribeDataChange(myvar);
    std::cout << "Got sub handle: " << handle << ", sleeping 5 sconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Disconnecting" << std::endl;
    client.Disconnect();
    return 0;
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown error." << std::endl;
  }
  return -1;
}

