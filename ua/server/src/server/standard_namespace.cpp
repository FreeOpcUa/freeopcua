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



  class StandardNamespace : public ViewServices
  {
  public:
    StandardNamespace()
    {
      Fill();
    }
    virtual std::vector<ReferenceDescription> Browse(const BrowseParameters& params)
    {
      return std::vector<ReferenceDescription>();
    }

    virtual std::vector<ReferenceDescription> BrowseNext()
    {
      return std::vector<ReferenceDescription>();
    }

  private:
  void AddReference(
    ObjectID sourceNode, 
    ObjectID targetNode, 
    bool isForward, 
    const char* name, 
    NodeClass targetNodeClass,
    ObjectID typeDefinition)
  {
    ReferenceDescription desc;
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

   AddReference(ObjectID::RootFolder, ObjectID::FolderType,    forward, Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
   AddReference(ObjectID::RootFolder, ObjectID::ObjectsFolder, forward, Names::Objects,    NodeClass::Object,     ObjectID::ObjectsFolder);
   AddReference(ObjectID::RootFolder, ObjectID::TypesFolder,   forward, Names::Types,      NodeClass::Object,     ObjectID::ObjectsFolder);
   AddReference(ObjectID::RootFolder, ObjectID::ViewsFolder,   forward, Names::Views,      NodeClass::Object,     ObjectID::ObjectsFolder);
  }

  private:
    std::map<ObjectID, ReferenceDescription> Referencies;
  };

}

std::unique_ptr<OpcUa::Remote::ViewServices> OpcUa::CreateStandardNamespace()
{
  return std::unique_ptr<Remote::ViewServices>(new StandardNamespace());
}

