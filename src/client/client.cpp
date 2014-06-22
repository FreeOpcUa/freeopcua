/******************************************************************************
 *   Copyright (C) 2014-2014 by Sintef Raufoss Manufacturing                  *
 *   olivier.roulet@gmail.com                  *
 *                                          *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as          *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                          *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.              *
 *                                          *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                  *
 *   Free Software Foundation, Inc.,                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ******************************************************************************/

#include <opc/ua/server.h>
#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/common/uri_facade.h>
#include <opc/ua/server.h>


namespace OpcUa
{

  void RemoteClient::Connect()
  {
    Server = OpcUa::Remote::Connect(Endpoint);

    OpcUa::Remote::SessionParameters session;
    session.ClientDescription.URI = Uri;
    session.ClientDescription.ProductURI = Uri;
    session.ClientDescription.Name.Text = SessionName;
    session.ClientDescription.Type = OpcUa::ApplicationType::CLIENT;
    session.SessionName = SessionName;
    session.EndpointURL = Endpoint;
    session.Timeout = 1200000;

    Server->CreateSession(session);
    Server->ActivateSession();
  }

  void RemoteClient::Disconnect()
  {
    if (  Server ) 
    {
      Server->CloseSession();
    }
    Server.reset(); //SecureChannel is not closed until we destroy server object
  }

  Node RemoteClient::GetNode(NodeID nodeId) const
  {
    if ( ! Server ) { throw NotConnectedError();}
    return Node(Server, nodeId);
  }

  Node RemoteClient::GetRoot() const
  {
    if ( ! Server ) { throw NotConnectedError();}
    return Node(Server, OpcUa::ObjectID::RootFolder);
  }

  Node RemoteClient::GetObjectsFolder() const
  {
    if ( ! Server ) { throw NotConnectedError();}
    return Node(Server, OpcUa::ObjectID::ObjectsFolder);
  }

} // namespace OpcUa

