/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa client command line options parser.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/ua/attribute_ids.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/data_value.h>

#include <string>

namespace OpcUa
{

  class CommandLine
  {
  public:
    CommandLine(int argc, char** argv);

    std::string GetServerURI() const
    {
      return ServerURI;
    }

    NodeID GetNodeID() const
    {
      return Node;
    }

    uint16_t GetNamespaceIndex() const
    {
      return NamespaceIndex;
    }

    Variant GetValue() const
    {
      return Value;
    }

    AttributeID GetAttribute() const
    {
      return Attribute;
    }

    bool IsGetEndpointsOperation() const
    {
      return IsGetEndpoints;
    }

    bool IsBrowseOperation() const
    {
      return IsBrowse;
    }

    bool IsReadOperation() const
    {
      return IsRead;
    }

    bool IsWriteOperation() const
    {
      return IsWrite;
    }

    bool IsCreateSubscriptionOperation() const
    {
      return IsCreateSubscription;
    }

    bool IsFindServersOperation() const
    {
      return IsFindServers;
    }

  private:
    std::string ServerURI;
    NodeID Node;
    uint16_t NamespaceIndex;
    Variant Value;
    AttributeID Attribute;
    bool IsGetEndpoints;
    bool IsBrowse;
    bool IsRead;
    bool IsWrite;
    bool IsCreateSubscription;
    bool IsFindServers;
  };

}

