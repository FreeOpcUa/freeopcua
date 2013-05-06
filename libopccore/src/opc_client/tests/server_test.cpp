/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of OpcBrowse inteerfaces
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opc/common/exception.h>
#include <opc/opc_client/server.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include <tr1/functional>

class ServerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ServerTestCase);
  CPPUNIT_TEST(TestGroupManagement);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestGroupManagement();
};

CPPUNIT_TEST_SUITE_REGISTRATION( ServerTestCase );

void ServerTestCase::TestGroupManagement()
{
  OpcClient::Server::UniquePtr server;
  CPPUNIT_ASSERT_NO_THROW(server = OpcClient::CreateServer());
  std::shared_ptr<OpcClient::Group> group;
  // adding group
  CPPUNIT_ASSERT_NO_THROW(group = server->AddGroup("private"));
  CPPUNIT_ASSERT(group);
  OpcClient::GroupProperties::SharedPtr groupProperties = group->GetProperties();
  CPPUNIT_ASSERT(groupProperties);
  
  // IsActive property 
  CPPUNIT_ASSERT(groupProperties->IsActive() == false);
  CPPUNIT_ASSERT_NO_THROW(groupProperties->SetIsActive(true));
  CPPUNIT_ASSERT(groupProperties->IsActive() == true);
  // TimeBias group property
  CPPUNIT_ASSERT(groupProperties->GetTimeBias() == 0);
  CPPUNIT_ASSERT_NO_THROW(groupProperties->SetTimeBias(1000));
  CPPUNIT_ASSERT(groupProperties->GetTimeBias() == 1000);
  // UpdateRate group property
  CPPUNIT_ASSERT(groupProperties->GetUpdateRate() == 0);
  CPPUNIT_ASSERT_NO_THROW(groupProperties->SetUpdateRate(1000));
  CPPUNIT_ASSERT(groupProperties->GetUpdateRate() == 1000);
  // Deadband group property
  CPPUNIT_ASSERT(groupProperties->GetDeadband() == 0.0);
  CPPUNIT_ASSERT_NO_THROW(groupProperties->SetDeadband(1000));
  CPPUNIT_ASSERT(groupProperties->GetDeadband() == 1000.0);
  

  CPPUNIT_ASSERT(server->GetPublicGroups().size() == 1);
  CPPUNIT_ASSERT_NO_THROW(group->SetIsPrivate(true));
  CPPUNIT_ASSERT(group->GetIsPrivate() == true);
  CPPUNIT_ASSERT(server->GetPublicGroups().size() == 0);
  CPPUNIT_ASSERT_THROW(server->AddGroup("private"), Common::Error);
  CPPUNIT_ASSERT_NO_THROW(group = server->AddGroup("public"));
  CPPUNIT_ASSERT(group);
  CPPUNIT_ASSERT(group->GetName() == "public");
  CPPUNIT_ASSERT(group->GetIsPrivate() == false);
  // changing group name
  CPPUNIT_ASSERT_NO_THROW(group->SetName("public2"));
  CPPUNIT_ASSERT(group->GetName() == "public2");
  // changing group name cannot no duplicate name - it should now only server.
  CPPUNIT_ASSERT_NO_THROW(group->SetName("public"));
  CPPUNIT_ASSERT(group->GetName() == "public");

  CPPUNIT_ASSERT_NO_THROW(server->GetGroupByName("private"));
  CPPUNIT_ASSERT_NO_THROW(server->GetGroupByName("public"));
  CPPUNIT_ASSERT(server->GetGroupByName("private") != server->GetGroupByName("public"));

  CPPUNIT_ASSERT(server->GetPrivateGroups().size() == 1);
  CPPUNIT_ASSERT(server->GetPublicGroups().size() == 1);
  CPPUNIT_ASSERT_NO_THROW(server->RemoveGroup("private"));
  CPPUNIT_ASSERT_THROW(server->GetGroupByName("private"), Common::Error);
  CPPUNIT_ASSERT_NO_THROW(server->GetGroupByName("public"));

  CPPUNIT_ASSERT(server->GetPublicGroups().size() == 1);
  CPPUNIT_ASSERT(server->GetPrivateGroups().size() == 0);

  CPPUNIT_ASSERT_NO_THROW(server->RemoveGroup("public"));
  CPPUNIT_ASSERT_THROW(server->GetGroupByName("public"), Common::Error);
  CPPUNIT_ASSERT(server->GetPrivateGroups().size() == 0);
  CPPUNIT_ASSERT(server->GetPublicGroups().size() == 0);
}
