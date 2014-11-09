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

#include <boost/chrono.hpp>
#include <opc/ua/server/addons/services_registry.h>
#include <functional>

#ifdef WIN32
#undef GetObject
#endif

namespace
{
  OpcUa::RelativePathElement GetHierarchicalElement(const std::string& browseName)
  {
    OpcUa::RelativePathElement element;
    element.ReferenceTypeID = OpcUa::ObjectID::HierarchicalReferences;
    element.IncludeSubtypes = true;
    element.TargetName.Name = browseName;
    return element;
  }


  OpcUa::RelativePath GetCurrentTimeRelativepath()
  {
    OpcUa::RelativePath path;
    path.Elements.push_back(GetHierarchicalElement(OpcUa::Names::ServerStatus));
    path.Elements.push_back(GetHierarchicalElement(OpcUa::Names::CurrentTime));
    return path;
  }
}

namespace OpcUa
{
  namespace Server
  {

    ServerObject::ServerObject(Services::SharedPtr services, boost::asio::io_service& io, bool debug)
      : Server(services)
      , Io(io)
      , Debug(debug)
      , Instance(std::move(CreateServerObject(services)))
      , ServerTime(Instance.GetVariable(GetCurrentTimeRelativepath()))
      , Timer(io)
    {
      Timer.Start(boost::posix_time::seconds(1), [this](){
        UpdateTime();
      });
    }

    ServerObject::~ServerObject()
    {
      if (Debug) std::clog << "server_object| canceling timer..." << std::endl;
      Timer.Cancel();
      if (Debug) std::clog << "server_object| timer stopped." << std::endl;
    }

    Model::Object ServerObject::CreateServerObject(const Services::SharedPtr& services) const
    {
      Model::Server server(services);
      Model::Object root = server.GetObject(ObjectID::ObjectsFolder);
      Model::ObjectType serverType = server.GetObjectType(ObjectID::ServerType);
      return root.CreateObject(ObjectID::Server, serverType, QualifiedName(OpcUa::Names::Server));
    }

    void ServerObject::UpdateTime()
    {
      try
      {
        DateTime t = OpcUa::CurrentDateTime();
        DataValue timeData(t);
        timeData.SetSourceTimestamp(t);
        timeData.SetServerTimestamp(t);

        if (Debug) std::clog << "server_object| Updating server time: " << t << std::endl;
        ServerTime.SetValue(timeData);
      }
      catch (std::exception& ex)
      {
        std::cerr << "Failed to update time at server object: " << ex.what() << std::endl;
      }

    }

  } // namespace UaServer
} // namespace OpcUa
