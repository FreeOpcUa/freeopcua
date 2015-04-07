/// @author Olivier Roulet-Dubonnet 2014
/// @brief Opc Ua Binary. Subscription service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/node_management.h>

#include <algorithm>
#include <stdexcept>


class AddNodesSerialization : public OpcUaBinarySerialization
{
};

class AddNodesDeSerialization : public OpcUaBinarySerialization
{
};

//-------------------------------------------------------
// AddnodesItem
//-------------------------------------------------------

TEST_F(AddNodesSerialization, AddNodesItem)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;


  AddNodesItem item;
  item.BrowseName = OpcUa::QualifiedName("titi", 2);
  item.ParentNodeId = OpcUa::NodeId(85, 0);
  item.RequestedNewNodeId = OpcUa::NodeId(99, 3);
  item.Class = NodeClass::Variable;
  item.ReferenceTypeId = ReferenceId::HasComponent; 
  item.TypeDefinition = ObjectId::BaseVariableType; 
  VariableAttributes attr;
  attr.DisplayName = LocalizedText(item.BrowseName.Name);
  attr.Description = LocalizedText(item.BrowseName.Name);
  attr.WriteMask = 0;
  attr.UserWriteMask = 0;
  attr.Value = 7;
  attr.Type = ObjectId::UInt32;
  attr.Rank  = 0;
  //attr.Dimensions = 0;
  attr.AccessLevel = VariableAccessLevel::CurrentRead;
  attr.UserAccessLevel = VariableAccessLevel::CurrentWrite;
  attr.MinimumSamplingInterval = 1;
  attr.Historizing = true;
  item.Attributes = attr;


  GetStream() << item << flush;

  const std::vector<char> expectedData = {
  2 ,0 ,0 ,(char)0x55 ,0 ,0 ,0 , //parent node id
  2 ,0 ,0 ,(char)0x2f ,0 ,0 ,0 , //referencetypeid
  2 ,3 ,0 ,(char)0x63 ,0 ,0 ,0 , //requested nodeid
  2 ,0 ,4 ,0 ,0 ,0 ,(char)0x74 ,(char)0x69 ,(char)0x74 ,(char)0x69 , //browsename
  2 ,0 ,0 ,0 , //Node class
  //start NodeAttributes
    1 ,0 ,(char)0x65 ,1 ,//TypeId
    1 , //encoding
    //start attributes
      (char)0x3d ,0 ,0 ,0 , //size
      (char)0x73 ,(char)0x12 ,(char)0x3d ,0 , //Specified Attributes
      2 ,4 ,0 ,0 ,0 ,(char)0x74 ,(char)0x69 , (char)0x74 ,(char)0x69 ,//disaply name
      2 ,4 ,0 ,0 ,0,(char)0x74 ,(char)0x69 ,(char)0x74 ,(char)0x69 , //description
      0 ,0 ,0 ,0 , //writemask
      0 ,0 ,0 ,0 ,//Userwritemask
      //variant
        6, //variant type
        7, 0, 0 ,0, //variant value
      2, 0 ,0 ,7 ,0 ,0, 0, //DataType
      0, 0, 0, 0, //value rank
      (char)0xff ,(char)0xff ,(char)0xff ,(char)0xff , //array of simple type ??
      1 , //access
      2 , //user access level
      0 , 0 , 0 ,0 , 0, 0, (char)0xf0, (char)0x3f, //Minimum sampleing rate
      1, //historyzing
      2, 0, 0, (char)0x3e, 0 , 0, 0 //type definition
  };


  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(item));
}

