/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_VIEW_H
#define OPC_UA_CLIENT_VIEW_H

#include <opc/common/interface.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/view.h>

#include <vector>

namespace OpcUa
{
  namespace Remote
  {

    class ViewServices : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(ViewServices);

    public:
      virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const = 0;
      virtual std::vector<ReferenceDescription> BrowseNext() const = 0;
      virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const = 0;
    };

  } // namespace Remote
} // namespace OpcUa

#endif // OPC_UA_CLIENT_VIEW_H

