#include "opc/ua/server_operations.h"

namespace OpcUa
{
ServerOperations::ServerOperations(Services::SharedPtr Server) : Server(Server)
{
}

void ServerOperations::WriteAttributes(std::vector<WriteValue> & updates)
{
  std::vector<StatusCode> codes = Server->Attributes()->Write(updates);
  CheckStatusCode(codes.front());
}

std::vector<DataValue> ServerOperations::ReadAttributes(std::vector<ReadValueId> & attributes)
{
  ReadParameters params;
  params.AttributesToRead = attributes;
  auto vec = Server->Attributes()->Read(params);
  return vec;
}

std::vector<DataValue> ServerOperations::ReadAttributes(std::vector<Node> & nodes, AttributeId attr)
{
  std::vector<ReadValueId> request;

  for (auto & n : nodes)
    {
      ReadValueId r;
      r.NodeId = n.GetId();
      r.AttributeId = attr;
      request.push_back(r);
    }

  return ReadAttributes(request);
}

std::vector<Node> ServerOperations::RegisterNodes(std::vector<Node> & nodes)
{
  std::vector<OpcUa::NodeId> ids;

  for (auto & node : nodes)
    {
      ids.push_back(node.GetId());
    }

  auto result = Server->Views()->RegisterNodes(ids);
  auto rit = result.begin();
  std::vector<Node> out;

  for (auto nit = nodes.begin(); nit != nodes.end() && rit != result.end(); ++nit, ++rit)
    {
      out.push_back(Node(nit->GetServices(), *rit));
    }

  return out;
}

void ServerOperations::UnregisterNodes(std::vector<Node> & nodes)
{
  std::vector<OpcUa::NodeId> ids;

  for (auto & node : nodes)
    {
      ids.push_back(node.GetId());
    }

  Server->Views()->UnregisterNodes(ids);
}
}
