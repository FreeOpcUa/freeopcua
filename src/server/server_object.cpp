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

#include "server_object.h"

#include <opc/ua/model.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/node.h>


namespace OpcUa
{
  namespace Server
  {

    ServerObject::ServerObject(Services::SharedPtr services)
    {
      Model::Server server(services);
      Model::Object root = server.RootObject();
      Model::ObjectType serverType = server.GetObjectType(ObjectID::ServerType);
      Model::Object serverObject = root.CreateObject(serverType);
//      CreateServerObject(*services->NodeManagement());
    }

    void ServerObject::CreateServerObject(OpcUa::NodeManagementServices& services)
    {
      OpcUa::AddNodesItem serverNode;
      serverNode.BrowseName = OpcUa::QualifiedName(OpcUa::Names::Server);
      serverNode.Class = OpcUa::NodeClass::Object;
      serverNode.ParentNodeId = OpcUa::ObjectID::ObjectsFolder;
      serverNode.ReferenceTypeId = OpcUa::ObjectID::Organizes;
      serverNode.TypeDefinition = OpcUa::ObjectID::ServerType;
      OpcUa::ObjectAttributes attr;
      attr.Description = OpcUa::LocalizedText(OpcUa::Names::Server);
      attr.DisplayName = OpcUa::LocalizedText(OpcUa::Names::Server);
      serverNode.Attributes = attr;
      const std::vector<OpcUa::AddNodesResult>& results = services.AddNodes({serverNode});
      if (results.size() != 1)
      {
        throw std::runtime_error("Address space error! Cannot add server object root node: returned wrong number of nodes.");
      }
      CheckStatusCode(results[0].Status);

      ServerCapabilities(results[0].AddedNodeID, services);
    }

    void ServerObject::ServerCapabilities(const OpcUa::NodeID& parent, OpcUa::NodeManagementServices& services)
    {
      // Attributes
      OpcUa::AddNodesItem node;
      node.RequestedNewNodeID = OpcUa::ObjectID::ServerCapabilities;
      node.BrowseName = OpcUa::QualifiedName(0, OpcUa::Names::ServerCapabilities);
      node.Class = OpcUa::NodeClass::Object;
      node.ParentNodeId = parent;
      node.ReferenceTypeId = OpcUa::ReferenceID::HasComponent;
      node.TypeDefinition = OpcUa::ObjectID::ServerCapabilitiesType;
      OpcUa::ObjectAttributes attrs;
      attrs.Description = OpcUa::LocalizedText(OpcUa::Names::ServerCapabilities);
      attrs.DisplayName = OpcUa::LocalizedText(OpcUa::Names::ServerCapabilities);
      node.Attributes = attrs;
      services.AddNodes(std::vector<OpcUa::AddNodesItem>{node});
    }
  } // namespace UaServer
} // namespace OpcUa
