/// @author Alexander Rykovanov 2014
/// @email rykovanov.as@gmail.com
/// @brief Subscription service addon.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Server
  {

    const char SubscriptionServiceAddonId[] = "subscriptions";

    class SubscriptionServiceAddonFactory : public Common::AddonFactory
    {
    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

  }
}
