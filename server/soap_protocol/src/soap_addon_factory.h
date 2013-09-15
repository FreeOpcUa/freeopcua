/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include "soap_addon.h"

#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Server
  {

    class SoapFactory : public Common::AddonFactory
    {
    public:
      virtual Common::Addon::UniquePtr CreateAddon()
      {
        return Common::Addon::UniquePtr(new Impl::SoapAddon());
      }
    };

  } // namespace Server
} // namespace OpcUa
