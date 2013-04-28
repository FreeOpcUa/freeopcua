/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

#include "standard_namespace.h"

#include <opc/ua/node_classes.h>
#include <opc/ua/strings.h>

#include <map>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Remote;

  typedef std::multimap<ObjectID, ReferenceDescription> ReferenciesMap;

  bool IsSuitableReference(const BrowseDescription& desc, const ReferenciesMap::value_type& refPair)
  {
    const ObjectID sourceNode = refPair.first;
    if (desc.NodeToBrowse != sourceNode)
    {
      return false;
    }
    const ReferenceDescription reference = refPair.second;
    if ((desc.Direction == BrowseDirection::Forward && !reference.IsForward) || (desc.Direction == BrowseDirection::Inverse && reference.IsForward))
    { 
      return false;
    }
    if (desc.ReferenceTypeID != reference.TypeID)
    {
      return false;
    }
    if ((desc.NodeClasses & static_cast<uint32_t>(reference.TargetNodeClass)) == 0)
    {
      return false;
    }
    return true;
  }

  class StandardNamespace : public ViewServices
  {
  public:
    StandardNamespace()
    {
      Fill();
    }

    virtual std::vector<ReferenceDescription> Browse(const BrowseParameters& params)
    {
      std::vector<ReferenceDescription> result;
      for (auto reference : Referencies)
      {
        if (IsSuitableReference(params.Description, reference))
        {
          result.push_back(reference.second);
        }
      }
      return result;
    }

    virtual std::vector<ReferenceDescription> BrowseNext()
    {
      return std::vector<ReferenceDescription>();
    }

  private:

  void AddReference(
    ObjectID sourceNode, 
    ObjectID targetNode, 
    ReferenceID referenceType,
    bool isForward, 
    const char* name, 
    NodeClass targetNodeClass,
    ObjectID typeDefinition)
  {
    ReferenceDescription desc;
    desc.TypeID = referenceType;
    desc.IsForward = isForward;
    desc.TargetNodeID = NodeID(targetNode);
    desc.BrowseName.Name = name;
    desc.DisplayName.Text = name;
    desc.TargetNodeClass = targetNodeClass; 
    desc.TypeDefinition = NodeID(typeDefinition);
   
    Referencies.insert({sourceNode, desc});
  }

  void Fill()
  {
   const bool forward = true;
   const bool reverse = true;

   AddReference(ObjectID::RootFolder, ObjectID::FolderType,    ReferenceID::HasTypeDefinition, forward, Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
   AddReference(ObjectID::RootFolder, ObjectID::ObjectsFolder, ReferenceID::Organizes,         forward, Names::Objects,    NodeClass::Object,     ObjectID::ObjectsFolder);
   AddReference(ObjectID::RootFolder, ObjectID::TypesFolder,   ReferenceID::Organizes,         forward, Names::Types,      NodeClass::Object,     ObjectID::ObjectsFolder);
   AddReference(ObjectID::RootFolder, ObjectID::ViewsFolder,   ReferenceID::Organizes,         forward, Names::Views,      NodeClass::Object,     ObjectID::ObjectsFolder);
  }

  private:
    ReferenciesMap Referencies;
  };

}

std::unique_ptr<OpcUa::Remote::ViewServices> OpcUa::CreateStandardNamespace()
{
  return std::unique_ptr<Remote::ViewServices>(new StandardNamespace());
}

