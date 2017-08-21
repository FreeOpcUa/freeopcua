/// @brief OPC UA Server main.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
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
  void DataChange(uint32_t handle, const Node & node, const Variant & val, AttributeId attr) override
  {
    std::cout << "Received DataChange event for Node " << node << std::endl;
  }
};

std::vector<OpcUa::Variant> MyMethod(NodeId context, std::vector<OpcUa::Variant> arguments)
{
  std::cout << "MyMethod called! " << std::endl;
  std::vector<OpcUa::Variant> result;
  result.push_back(Variant(static_cast<uint8_t>(0)));
  return result;
}

void RunServer()
{
  //First setup our server
  auto logger = spdlog::stderr_color_mt("server");
  OpcUa::UaServer server(logger);
  server.SetEndpoint("opc.tcp://localhost:4840/freeopcua/server");
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
  Node mymethod = newobject.AddMethod(idx, "MyMethod", MyMethod);

  //browse root node on server side
  Node root = server.GetRootNode();
  logger->info("Root node is: {}", root);
  logger->info("Children are:");

  for (Node node : root.GetChildren())
    {
      logger->info("    {}", node);
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


  logger->info("Ctrl-C to exit");

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

int main(int argc, char ** argv)
{
  try
    {
      RunServer();
    }

  catch (const std::exception & exc)
    {
      std::cout << exc.what() << std::endl;
    }

  return 0;
}

