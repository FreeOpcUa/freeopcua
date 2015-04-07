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

#include "model_impl.h"

#include <opc/ua/model.h>

namespace OpcUa
{
  namespace Model
  {

    ObjectType::ObjectType(NodeId objectId, Services::SharedPtr services)
      : Node(services)
    {
      Id = objectId;
      ReadParameters attrs;
      attrs.AttributesToRead.push_back(ToReadValueId(objectId, AttributeId::DisplayName));
      attrs.AttributesToRead.push_back(ToReadValueId(objectId, AttributeId::BrowseName));
      attrs.AttributesToRead.push_back(ToReadValueId(objectId, AttributeId::IsAbstract));
      std::vector<DataValue> values = services->Attributes()->Read(attrs);
      DisplayName = values[0].Value.As<LocalizedText>();
      BrowseName = values[1].Value.As<QualifiedName>();
      Abstract = values[2].Value.As<bool>();
    }

    bool ObjectType::IsAbstract() const
    {
      return false;
    }

    std::vector<Variable> ObjectType::Variables() const
    {
      return Browse<Variable>(GetId(), NodeClass::Variable, GetServices());
    }

    std::vector<Object> ObjectType::Objects() const
    {
      return Browse<Object>(GetId(), NodeClass::Object, GetServices());
    }

    std::vector<ObjectType> ObjectType::SubTypes() const
    {
      return Browse<ObjectType>(GetId(), NodeClass::ObjectType, GetServices());
    }

    ObjectType ObjectType::Parent() const
    {
      return ObjectType(ObjectId::Null, GetServices());
    }

  }
}
