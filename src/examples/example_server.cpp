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
#include <opc/ua/server/server.h>




using namespace OpcUa;

class SubClient : public SubscriptionHandler
{
  void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attr) const override
  {
    std::cout << "Received DataChange event for Node " << node << std::endl;
  }
};

void RunServer()
{
  //First setup our server
  const bool debug = true;
  OpcUa::UaServer server(debug);
  server.SetEndpoint("opc.tcp://localhost:4841/freeopcua/server");
  server.SetServerURI("urn://exampleserver.freeopcua.github.io");
  server.Start();
  
  //then register our server namespace and get its index in server
  uint32_t idx = server.RegisterNamespace("http://examples.freeopcua.github.io");
  
  //Create our address space using different methods
  Node objects = server.GetObjectsNode();
  
  //Add a custom object with specific nodeid
  NodeId nid(99, idx);
  QualifiedName qn("NewObject", idx);
  Node newobject = objects.AddObject(nid, qn);

  //Add a variable and a property with auto-generated nodeid to our custom object
  Node myvar = newobject.AddVariable(idx, "MyVariable", Variant(8));
  Node myprop = newobject.AddVariable(idx, "MyProperty", Variant(8.8));

  //browse root node on server side
  Node root = server.GetRootNode();
  std::cout << "Root node is: " << root << std::endl;
  std::cout << "Childs are: " << std::endl;
  for (Node node : root.GetChildren())
  {
    std::cout << "    " << node << std::endl;
  }


  //Uncomment following to subscribe to datachange events inside server
  /*
  SubClient clt;
  std::unique_ptr<Subscription> sub = server.CreateSubscription(100, clt);
  sub->SubscribeDataChange(myvar);
  */



  //Now write values to address space and send events so clients can have some fun
  uint32_t counter = 0;
  myvar.SetValue(Variant(counter)); //will change value and trigger datachange event

  //Create event
  server.EnableEventNotification();
  Event ev(ObjectId::BaseEventType); //you should create your own type
  ev.Severity = 2;
  ev.SourceNode = ObjectId::Server;
  ev.SourceName = "Event from FreeOpcUA";
  ev.Time = DateTime::Current();


  std::cout << "Ctrl-C to exit" << std::endl;
  for (;;)
  {
    myvar.SetValue(Variant(++counter)); //will change value and trigger datachange event
    std::stringstream ss;
    ss << "This is event number: " << counter;
    ev.Message = LocalizedText(ss.str());
    server.TriggerEvent(ev);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  }

  server.Stop();
}

int main(int argc, char** argv)
{
  try
  {
    RunServer();
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  return 0;
}

