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

#include <opc/ua/client/remote_server.h>
#include <opc/ua/object_ids.h>

#include <stdexcept>

using namespace OpcUa;
using namespace OpcUa::Remote;


class View : public ::testing::Test
{
protected:
  View()
  {
    Query.Description.NodeToBrowse.Encoding = EV_TWO_BYTE;
    Query.Description.NodeToBrowse.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::RootFolder); // TODO automatic cast
    Query.Description.Direction = BrowseDirection::Forward;
    Query.Description.ReferenceTypeID.Encoding = EV_TWO_BYTE;
//    Params.Description.ReferenceTypeID.TwoByteData.Identifier = 33;
    Query.Description.IncludeSubtypes = true;
    Query.Description.NodeClasses = NODE_CLASS_ALL;
    Query.Description.ResultMask = REFERENCE_ALL;
    Query.MaxReferenciesCount = 1;
  }

  virtual void SetUp()
  {
    Server = Connect(GetEndpoint());

    SessionParameters session;
    session.ClientDescription.Name.Text = "opcua view service test";
    session.SessionName = "opua test";
    session.EndpointURL = GetEndpoint();
    session.Timeout = 1000;

    Server->CreateSession(session);
    Server->ActivateSession();

    Service = Server->Views();
  }

  virtual void TearDown()
  {
    Service = std::unique_ptr<ViewServices>();
    if (Server)
    {
      Server->CloseSession();
    }
    Server.reset();
  }

protected:
  OpcUa::NodesQuery Query;
  std::unique_ptr<Server> Server;
  std::shared_ptr<ViewServices> Service;
};



TEST_F(View, Browse)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));

  const std::vector<ReferenceDescription> refs =  Service->Browse(Query);
  ASSERT_EQ(refs.size(), 1);
}

TEST_F(View, BrowseNext)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));

  Service->Browse(Query);
  const std::vector<ReferenceDescription> refs =  Service->BrowseNext();
  ASSERT_EQ(refs.size(), 1);
}

TEST_F(View, BrowseFinishes)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));

  Service->Browse(Query);
  std::vector<ReferenceDescription> browsedRefs;
  unsigned count = 0;
  while (true)
  {
    const std::vector<ReferenceDescription> refs = Service->BrowseNext();
    if (refs.empty())
    {
      break;
    }
    browsedRefs.insert(browsedRefs.end(), refs.begin(), refs.end());
    ++count;
  }
  EXPECT_TRUE(count > 0);
  EXPECT_EQ(browsedRefs.size(), count);
}

