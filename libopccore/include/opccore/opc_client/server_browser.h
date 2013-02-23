/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Interface for browsing server groups items ans it propertires.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __server_browser_h_077c354b_0cf8_4fdf_9de3_f191e8a1b820
#define __server_browser_h_077c354b_0cf8_4fdf_9de3_f191e8a1b820

#include <opccore/common/class_pointers.h>
#include <opccore/common/noncopyable.h>
#include <opccore/common/value.h>
#include <opccore/common/object_id.h>

#include <string>
#include <vector>

namespace OpcClient
{
  struct Property
  {
    unsigned long ID;
    std::string ItemID;
    std::string Description;
    Common::Value Value;
  };

  struct BrowseElement
  {
    std::string Name;
    std::string ID;
    bool HasChildren;
    bool IsItem;
    std::vector<Property> Properties;
  };

  enum BrowseFilter
  {
    OPC_BROWSE_FILTER_ALL      = 1,
    OPC_BROWSE_FILTER_BRANCHES = 2,
    OPC_BROWSE_FILTER_ITEMS    = 3,
  };

  class ServerBrowser
    : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(ServerBrowser);

  public:
    virtual std::vector<BrowseElement> Browse(
      const std::string& elementPath,
      BrowseFilter filter,
      const std::string& nameFilter,
      const std::string& regexFilter,
      bool returnAllProperties,
      bool returnAllPropertiesValues,
      const std::vector<Common::ObjectID>& propertiesIDs) const = 0;
 };

  ServerBrowser::UniquePtr CreateServerBrowser();
}

#endif // __server_browser_h_077c354b_0cf8_4fdf_9de3_f191e8a1b820
