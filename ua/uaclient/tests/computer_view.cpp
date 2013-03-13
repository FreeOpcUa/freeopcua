/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test browseing with computer.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/client/computer.h>
#include <opc/ua/object_ids.h>

#include <stdexcept>

TEST(View, Works)
{
  using namespace OpcUa;
  std::unique_ptr<Remote::Computer> computer = Remote::Connect(GetEndpoint());
  std::shared_ptr<Remote::ViewServices> view = computer->Views();
  ASSERT_TRUE(static_cast<bool>(view));

  BrowseDescription desc;
  desc.NodeToBrowse.Encoding = EV_TWO_BYTE;
  desc.NodeToBrowse.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::RootFolder); // TODO automatic cast
  desc.Direction = BrowseDirection::FORWARD;
  desc.ReferenceTypeID.Encoding = EV_TWO_BYTE;
  desc.ReferenceTypeID.TwoByteData.Identifier = 33;
  desc.IncludeSubtypes = true;
  desc.NodeClasses = NODE_CLASS_ALL;
  desc.ResultMask = REFERENCE_ALL;

  view->Browse(desc);

}

