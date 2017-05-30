/// @author Alexander Rykovanov 2014
/// @email rykovanov.as@gmail.com
/// @brief Subscription service addon.
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

const char SubscriptionServiceAddonId[] = "subscriptions";

class SubscriptionServiceAddonFactory : public Common::AddonFactory
{
public:
  virtual Common::Addon::UniquePtr CreateAddon();
};

}
}
