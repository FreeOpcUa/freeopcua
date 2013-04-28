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

#include <opc/ua/client/remote_computer.h>
#include <opc/ua/object_ids.h>

#include <stdexcept>

using namespace OpcUa;
using namespace OpcUa::Remote;


class View : public ::testing::Test
{
protected:
  View()
  {
    Params.Description.NodeToBrowse.Encoding = EV_TWO_BYTE;
    Params.Description.NodeToBrowse.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::RootFolder); // TODO automatic cast
    Params.Description.Direction = BrowseDirection::Forward;
    Params.Description.ReferenceTypeID.Encoding = EV_TWO_BYTE;
//    Params.Description.ReferenceTypeID.TwoByteData.Identifier = 33;
    Params.Description.IncludeSubtypes = true;
    Params.Description.NodeClasses = NODE_CLASS_ALL;
    Params.Description.ResultMask = REFERENCE_ALL;
    Params.MaxReferenciesCount = 1;
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
  BrowseParameters Params;
  std::unique_ptr<Computer> Server;
  std::shared_ptr<ViewServices> Service;
};



TEST_F(View, Browse)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));

  const std::vector<ReferenceDescription> refs =  Service->Browse(Params);
  ASSERT_EQ(refs.size(), 1);
}

TEST_F(View, BrowseNext)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));

  Service->Browse(Params);
  const std::vector<ReferenceDescription> refs =  Service->BrowseNext();
  ASSERT_EQ(refs.size(), 1);
}

TEST_F(View, BrowseFinishes)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));

  Service->Browse(Params);
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

