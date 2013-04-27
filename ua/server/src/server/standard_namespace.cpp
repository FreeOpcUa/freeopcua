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

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Remote;

  class StandardNamespace : public ViewServices
  {
  public:
    virtual std::vector<ReferenceDescription> Browse(const BrowseParameters& params)
    {
      return std::vector<ReferenceDescription>();
    }

    virtual std::vector<ReferenceDescription> BrowseNext()
    {
      return std::vector<ReferenceDescription>();
    }
  };

}

std::unique_ptr<OpcUa::Remote::ViewServices> OpcUa::CreateStandardNamespace()
{
  return std::unique_ptr<Remote::ViewServices>(new StandardNamespace());
}

