/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "address_space_internal.h"

#include <map>
#include <set>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Server;
  using namespace OpcUa::Remote;


  class ServicesRegistry : public Internal::AddressSpaceMultiplexor
  {
  public:
    virtual void Register(ViewServices::SharedPtr views)
    {
      Views.insert(views);
    }

    virtual void Unregister(ViewServices::SharedPtr views)
    {
      Views.erase(views);
    }

    virtual void Register(uint16_t namespaceIndex, AttributeServices::SharedPtr attributes)
    {
      Attributes.insert(std::make_pair(namespaceIndex, attributes));
    }

    virtual void Unregister(uint16_t namespaceIndex)
    {
      Attributes.erase(namespaceIndex);
    }

    virtual std::vector<ReferenceDescription> Browse(const BrowseParameters& params) const
    {
      std::vector<ReferenceDescription> result;
      for (ViewServices::WeakPtr viewIt : Views)
      {
        ViewServices::SharedPtr view = viewIt.lock();
        if (view)
        {
          std::vector<ReferenceDescription> tmp = view->Browse(params);
          result.insert(result.end(), tmp.begin(), tmp.end());
        }
      }
      return result;
    }

    virtual std::vector<ReferenceDescription> BrowseNext() const
    {
      std::vector<ReferenceDescription> result;
      for (ViewServices::WeakPtr viewIt : Views)
      {
        ViewServices::SharedPtr view = viewIt.lock();
        if (view)
        {
          std::vector<ReferenceDescription> tmp = view->BrowseNext();
          result.insert(result.end(), tmp.begin(), tmp.end());
        }
      }
      return result;
    }

    virtual std::vector<DataValue> Read(const ReadParameters& params) const
    {
      struct ReadID
      {
        std::size_t Index;
        AttributeValueID Attribute;
        DataValue Value;
      };

      std::vector<ReadID> ids;
      for (std::size_t index = 0; index < params.AttributesToRead.size(); ++index)
      {
        ReadID id;
        id.Index = index;
        id.Attribute = params.AttributesToRead.at(index);
      }

      std::vector<DataValue> values(params.AttributesToRead.size());
      for (auto attributeID : params.AttributesToRead)
      {

      }
      return std::vector<DataValue>();
    }

    virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& values)
    {
      return std::vector<StatusCode>();
    }

  private:
    std::set<ViewServices::SharedPtr> Views;
    std::map<uint16_t, AttributeServices::SharedPtr> Attributes;
  };

}

namespace OpcUa
{
  Internal::AddressSpaceMultiplexor::UniquePtr Internal::CreateAddressSpaceMultiplexor()
  {
    return AddressSpaceMultiplexor::UniquePtr(new ServicesRegistry());
  }
}
