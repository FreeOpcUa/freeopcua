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

#include <opc/ua/model.h>

namespace OpcUa
{
  namespace Model
  {
    template <typename T>
    std::vector<T> Browse(const NodeId& node, NodeClass nodeClassMask, Services::SharedPtr services)
    {
      BrowseDescription desc;
      desc.Direction = BrowseDirection::Forward;
      desc.IncludeSubtypes = true;
      desc.NodeClasses =   nodeClassMask;
      desc.ReferenceTypeId = ObjectId::HierarchicalReferences;
      desc.NodeToBrowse = node;
      desc.ResultMask = BrowseResultMask::None;

      NodesQuery query;
      query.NodesToBrowse.push_back(desc);
      ViewServices::SharedPtr views = services->Views();
      std::vector<BrowseResult> results = views->Browse(query);

      std::vector<T> objects;
      std::for_each(results[0].Referencies.begin(), results[0].Referencies.end(), [&node, &services, &objects](const ReferenceDescription& ref){
        objects.push_back(T(ref.TargetNodeId, services));
      });

      return objects;
    }
  }
}
