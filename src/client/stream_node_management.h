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

#pragma once

#include <opc/ua/node_management.h>
#include <opc/ua/protocol/binary/stream.h>

#include <stdexcept>
#include <iostream>

namespace OpcUa
{
  namespace Internal
  {

    template <typename StreamType>
    class NodeManagementServices : public OpcUa::Remote::NodeManagementServices
    {
    public:
    public:
      NodeManagementServices(std::shared_ptr<IOChannel> channel, const NodeID& sessionToken)
        : Stream(channel)
        , AuthenticationToken(sessionToken)
      {
      }

    public:
      virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
      {
        throw std::runtime_error("Deprecated method");
      }

      virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
      {
        throw std::runtime_error("Deprecated method");
      }

      virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items)
      {
        AddNodesRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters.NodesToAdd = items;
        
        Stream << request << OpcUa::Binary::flush;
        
        AddNodesResponse response;
        Stream >> response;
        
        return response.results;
      }

      virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items)
      {
        AddReferencesRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters.ReferencesToAdd = items;
        
        Stream << request << OpcUa::Binary::flush;
        
        AddReferencesResponse response;
        Stream >> response;
        
        return response.Results;
      }


    private:
      StreamType Stream;
      NodeID AuthenticationToken;
    };

  }
}
