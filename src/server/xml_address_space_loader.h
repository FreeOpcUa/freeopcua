/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once


#include <opc/common/addons_core/addon.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/services/node_management.h>
#include <opc/ua/server/addons/xml_ns.h>


namespace OpcUa
{
  namespace Internal
  {

    class XmlAddressSpaceLoader : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(XmlAddressSpaceLoader);

    public:
      explicit XmlAddressSpaceLoader(OpcUa::NodeManagementServices& registry, bool debug = false);

      void Load(const char* fileName);

      void Load(const std::string& fileName)
      {
        Load(fileName.c_str());
      }

    private:
      OpcUa::NodeManagementServices& Registry;
      const bool Debug;
    };


    class XmlAddressSpaceAddon : public OpcUa::Server::XmlAddressSpace
    {
    public:
      DEFINE_CLASS_POINTERS(XmlAddressSpaceAddon);

    public:
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

      virtual void Load(const char* path);

    private:
      void Load(const char* file, OpcUa::NodeManagementServices& registry);

    private:
      NodeManagementServices::SharedPtr Registry;
    };


    class XmlAddressSpaceAddonFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(XmlAddressSpaceAddonFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon()
      {
        return Common::Addon::UniquePtr(new XmlAddressSpaceAddon());
      }
    };

    const char XmlAddressSpaceAddonId[] = "xml_address_space_loader";

  } // namespace Internal
} // namespace OpcUa

