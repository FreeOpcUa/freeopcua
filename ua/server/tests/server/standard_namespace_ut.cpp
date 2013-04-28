/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <src/server/standard_namespace.h>

#include "common.h"

using namespace testing;
using namespace OpcUa;

TEST(StandardNamespace, CanBeCreated)
{
  std::unique_ptr<Remote::ViewServices> ns = CreateStandardNamespace();
  ASSERT_TRUE(static_cast<bool>(ns));
}

TEST(StandardNamespace, CanBrowseRootFolder)
{
  std::unique_ptr<Remote::ViewServices> ns = CreateStandardNamespace();
  OpcUa::Remote::BrowseParameters params;
  params.Description.NodeToBrowse = ObjectID::RootFolder;
  params.Description.Direction = BrowseDirection::Forward;
  params.Description.ReferenceTypeID = ReferenceID::Organizes;
  params.Description.IncludeSubtypes = true;
  params.Description.NodeClasses = NODE_CLASS_OBJECT;
  params.Description.ResultMask = REFERENCE_ALL;
  std::vector<ReferenceDescription> referencies = ns->Browse(params);
  ASSERT_EQ(referencies.size(), 3);
}

