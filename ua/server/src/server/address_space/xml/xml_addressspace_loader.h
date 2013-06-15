/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef OPC_UA_SERVER_XML_ADDRESS_SPACE_H
#define OPC_UA_SERVER_XML_ADDRESS_SPACE_H

#include <opc/common/addons_core/addon.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/address_space_registry.h>


namespace OpcUa
{
  namespace Internal
  {

    class XmlAddressSpaceLoader : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(XmlAddressSpaceLoader);

    public:
      explicit XmlAddressSpaceLoader(OpcUa::Server::AddressSpaceRegistry& registry);

      void Load(const char* fileName);

      void Load(const std::string& fileName)
      {
        Load(fileName.c_str());
      }

    private:
      OpcUa::Server::AddressSpaceRegistry& Registry;
    };


    class XmlAddressSpaceAddon : public Common::Addon
    {
    public:
      DEFINE_CLASS_POINTERS(XmlAddressSpaceAddon);

    public:
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();
    };


    class XmlAddressSpaceAddonFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(XmlAddressSpaceAddonFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon() const
      {
        return Common::Addon::UniquePtr(new XmlAddressSpaceAddon());
      }
    };

    const char XmlAddressSpaceAddonID[] = "xml_address_space_loader";

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_SERVER_XML_ADDRESS_SPACE_H
