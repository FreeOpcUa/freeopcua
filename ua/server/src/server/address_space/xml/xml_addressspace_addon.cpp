/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "xml_addressspace_loader.h"

namespace OpcUa
{
  namespace Internal
  {

    void XmlAddressSpaceAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      Server::AddressSpaceRegistry::SharedPtr registry = Common::GetAddon<Server::AddressSpaceRegistry>(addons, Server::AddressSpaceRegistryAddonID);
      XmlAddressSpaceLoader xml(*registry);
      for (const Common::Parameter& param : params.Parameters)
      {
        if (param.Name == "file_name")
        {
          try
          {
            xml.Load(param.Value.c_str());
          }
          catch (const std::exception& err)
          {
            std::cerr << "Unable to load address space from the file '" << param.Value << "'. " << err.what() << std::endl;
          }
        }
      }
    }

    void XmlAddressSpaceAddon::Stop()
    {
    }

  } // namespace Internal
} // namespace OpcUa


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new OpcUa::Internal::XmlAddressSpaceAddon());
}
