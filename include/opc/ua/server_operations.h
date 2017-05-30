#pragma once

#include "opc/ua/services/services.h"
#include "opc/ua/node.h"

#include <vector>

namespace OpcUa
{
class UaClient;
class UaServer;

class ServerOperations
{
  friend UaClient;
  friend UaServer;

  Services::SharedPtr Server;
protected:
  ServerOperations(Services::SharedPtr Server);

public:
  void WriteAttributes(std::vector<WriteValue> &);
  std::vector<DataValue> ReadAttributes(std::vector<ReadValueId> & attributes);
  std::vector<DataValue> ReadAttributes(std::vector<Node> & nodes, AttributeId attr);
  std::vector<Node> RegisterNodes(std::vector<Node> &);
  //NB This makes the given nodes invalid
  void UnregisterNodes(std::vector<Node> &);
};
}