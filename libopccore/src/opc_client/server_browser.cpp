/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Server space browse.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opccore/opc_client/server_browser.h>

#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/property_tree/group.h>
#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/property_tree.h>
#include <tr1/functional>
#include <iostream>

namespace
{
  inline OpcClient::BrowseElement GetGroupBrowseElement(
    Gefest::Group::SharedPtr group, 
    bool returnProperties, 
    bool returnPropertiesValues,
    std::vector<Common::ObjectID> propertiesIDs)
  {
    OpcClient::BrowseElement element;
    element.Name = group->GetName();
    element.ID = group->GetObjectPath();
    element.HasChildren = !group->GetItems().empty() || !group->GetSubGroups().empty();
    element.IsItem = false;
    // TODO add standard properties
    return element;
  }

  inline OpcClient::BrowseElement GetItemBrowseElement(
    Gefest::Item::SharedPtr item, 
    bool returnProperties, 
    bool returnPropertiesValues,
    std::vector<Common::ObjectID> propertiesIDs)
  {
    OpcClient::BrowseElement element;
    element.Name = item->GetName();
    element.ID = item->GetObjectPath();
    element.HasChildren = false;
    element.IsItem = true;
    // TODO add Item Properties
    // TODO add standard properties
    return element;
  }

  class ServerBrowserImpl : public OpcClient::ServerBrowser
  {
  public:
    ServerBrowserImpl();

    virtual std::vector<OpcClient::BrowseElement> Browse(
      const std::string& elementPath,
      OpcClient::BrowseFilter filter,
      const std::string& nameFilter,
      const std::string& regexFilter,
      bool returnAllProperties,
      bool returnAllPropertiesValues,
      const std::vector<Common::ObjectID>& propertiesIDs) const;

  private:
     std::shared_ptr<Gefest::PropertyTree> PropertyTree;
  };

  ServerBrowserImpl::ServerBrowserImpl()
    : PropertyTree(Common::GetAddon< ::PropertyTree::Manager >(Common::ADDON_ID_PROPERTY_TREE)->GetPropertyTree())
  {
    if (!PropertyTree)
    {
      throw;
    }
  }

  std::vector<OpcClient::BrowseElement> ServerBrowserImpl::Browse(
    const std::string& elementPath,
    OpcClient::BrowseFilter filter,
    const std::string& nameFilter,
    const std::string& regexFilter,
    bool returnAllProperties,
    bool returnAllPropertiesValues,
    const std::vector<Common::ObjectID>& propertiesIDs) const
  {
    std::shared_ptr<Gefest::Group> group = elementPath.empty() ? PropertyTree->GetRootGroup() : PropertyTree->FindGroup(elementPath);
    if (!group)
    {
      return std::vector<OpcClient::BrowseElement>();
    }

    std::vector<OpcClient::BrowseElement> elements;
    if (filter == OpcClient::OPC_BROWSE_FILTER_ALL || filter == OpcClient::OPC_BROWSE_FILTER_BRANCHES)
    {
      const std::vector<Gefest::Group::SharedPtr>& subGroups = group->GetSubGroups();
      std::transform(subGroups.begin(), subGroups.end(), std::back_inserter(elements), 
        std::tr1::bind(
          GetGroupBrowseElement, 
          std::tr1::placeholders::_1, 
          returnAllProperties, 
          returnAllPropertiesValues,
          std::tr1::cref(propertiesIDs)));
    }

    if (filter == OpcClient::OPC_BROWSE_FILTER_ALL || filter == OpcClient::OPC_BROWSE_FILTER_ITEMS)
    {
      const std::vector<Gefest::Item::SharedPtr>& items = group->GetItems();
      std::transform(items.begin(), items.end(), std::back_inserter(elements), 
        std::tr1::bind(
          GetItemBrowseElement, 
          std::tr1::placeholders::_1, 
          returnAllProperties, 
          returnAllPropertiesValues,
          std::tr1::cref(propertiesIDs)));
    }
    return elements;
  }
} // namespace

OpcClient::ServerBrowser::UniquePtr OpcClient::CreateServerBrowser()
{
  return OpcClient::ServerBrowser::UniquePtr(new ServerBrowserImpl);
}
