/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#pragma once

#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
namespace Server
{

const char XmlAddressSpaceAddonId[] = "xml_address_space";

class XmlAddressSpace : public Common::Addon
{
public:
  virtual void Load(const char * path) = 0;
};

}
}

