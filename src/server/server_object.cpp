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

#include <opc/ua/server/addons/services_registry.h>


namespace OpcUa
{
  namespace Server
  {

    ServerObject::ServerObject(Services::SharedPtr services)
      : Server(services)
      , Instance(std::move(CreateServerObject(services)))
    {
    }

    Model::Object ServerObject::CreateServerObject(const Services::SharedPtr& services) const
    {
      Model::Server server(services);
      Model::Object root = server.GetObject(ObjectID::ObjectsFolder);
      Model::ObjectType serverType = server.GetObjectType(ObjectID::ServerType);
      return root.CreateObject(ObjectID::Server, serverType, QualifiedName(OpcUa::Names::Server));
    }

  } // namespace UaServer
} // namespace OpcUa
