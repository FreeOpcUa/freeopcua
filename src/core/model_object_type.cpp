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

    ObjectType::ObjectType(NodeID objectID, Services::SharedPtr services)
      : Node(objectID, services)
    {
    }

    bool ObjectType::IsAbstract() const
    {
      return false;
    }

    std::vector<Variable> ObjectType::Variables() const
    {
      return std::vector<Variable>();
    }

    std::vector<Object> ObjectType::Objects() const
    {
      return std::vector<Object>();
    }

    std::vector<ObjectType> ObjectType::SubTypes() const
    {
      return std::vector<ObjectType>();
    }

    ObjectType ObjectType::Parent() const
    {
      return ObjectType(ObjectID::Null, GetServices());
    }

  }
}
