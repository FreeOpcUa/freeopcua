/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "xml_address_space_loader.h"

#include <sstream>
#include <stdexcept>

namespace OpcUa
{
  namespace Internal
  {

    void XmlAddressSpaceAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      Registry = addons.GetAddon<UaServer::AddressSpaceRegistry>(UaServer::AddressSpaceRegistryAddonID);
      if (!Registry)
      {
        std::stringstream stream;
        stream << "Unable to find addon '" << UaServer::AddressSpaceRegistryAddonID << "'. " << std::endl;
        throw std::logic_error(stream.str());
      }

      for (const Common::Parameter& param : params.Parameters)
      {
        if (param.Name == "file_name")
        {
          try
          {
            Load(param.Value.c_str(), *Registry);
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
      Registry.reset();
    }

    void XmlAddressSpaceAddon::Load(const char* path)
    {
      Load(path, *Registry);
    }

    void XmlAddressSpaceAddon::Load(const char* file, OpcUa::UaServer::AddressSpaceRegistry& registry)
    {
      if (!Registry)
      {
        std::stringstream stream;
        stream << "Unable to find addon '" << UaServer::AddressSpaceRegistryAddonID << "'. " << std::endl;
        throw std::logic_error(stream.str());
      }

      XmlAddressSpaceLoader xml(*Registry);
      xml.Load(file);
    }

  } // namespace Internal
} // namespace OpcUa
