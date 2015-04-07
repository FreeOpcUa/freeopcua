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

#include <opc/ua/model.h>

namespace OpcUa
{
  namespace Model
  {
    Node::Node(NodeId id, Services::SharedPtr services)
      : Id(id)
      , OpcUaServices(services)
    {
      ReadParameters attrs;
      attrs.AttributesToRead.push_back(ToReadValueId(id, AttributeId::DisplayName));
      attrs.AttributesToRead.push_back(ToReadValueId(id, AttributeId::BrowseName));
      std::vector<DataValue> values = services->Attributes()->Read(attrs);
      DisplayName = values[0].Value.As<LocalizedText>();
      BrowseName = values[1].Value.As<QualifiedName>();
    }

    Node::Node(Services::SharedPtr services)
      : OpcUaServices(services)
    {
    }

    NodeId Node::GetId() const
    {
      return Id;
    }

    QualifiedName Node::GetBrowseName() const
    {
      return BrowseName;
    }

    LocalizedText Node::GetDisplayName() const
    {
      return DisplayName;
    }

    std::vector<Reference> Node::GetReferencies() const
    {
      return std::vector<Reference>();
    }

  }
}
