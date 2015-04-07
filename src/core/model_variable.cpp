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
    Variable::Variable(NodeId variableId, Services::SharedPtr services)
      : Node(services)
    {
      Id = variableId;
      ReadParameters attrs;
      attrs.AttributesToRead.push_back(ToReadValueId(variableId, AttributeId::DisplayName));
      attrs.AttributesToRead.push_back(ToReadValueId(variableId, AttributeId::BrowseName));
      attrs.AttributesToRead.push_back(ToReadValueId(variableId, AttributeId::DataType));
      std::vector<DataValue> values = services->Attributes()->Read(attrs);
      DisplayName = values[0].Value.As<LocalizedText>();
      BrowseName = values[1].Value.As<QualifiedName>();
      DataType = OpcUa::DataTypeToVariantType(values[2].Value.As<NodeId>());
    }

    DataValue Variable::GetValue() const
    {
      ReadParameters params;
      params.AttributesToRead.push_back(ToReadValueId(GetId(), AttributeId::Value));
      const std::vector<DataValue> result = GetServices()->Attributes()->Read(params);
      if (result.size() != 1)
      {
        throw std::runtime_error("Cannot read variable value. Server returned invalid number of values.");
      }
      return result.front();
    }

    void Variable::SetValue(const Variant& value)
    {
      DataValue data(value);
      data.SetSourceTimestamp(OpcUa::DateTime::Current());
      SetValue(data);
    }

    void Variable::SetValue(const DataValue& value)
    {
      WriteValue writeValue;
      writeValue.AttributeId = AttributeId::Value;
      writeValue.Value = value;
      writeValue.NodeId = Id;
      std::vector<StatusCode> result = GetServices()->Attributes()->Write({writeValue});
      if (result.size() != 1)
      {
        throw std::runtime_error("Failed to write data. Server returned wron nunber of status codes.");
      }
      CheckStatusCode(result[0]);
    }

    std::vector<Variable> Variable::Variables() const
    {
      return Browse<Variable>(GetId(), NodeClass::Variable, GetServices());
    }
  }
}
