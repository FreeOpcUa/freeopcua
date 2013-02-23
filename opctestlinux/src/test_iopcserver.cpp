/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tests for com server interfaces.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "com_server_fixture.h"
#include <opc/opcda.h>
namespace
{
  class OpcServer : public OPCTest::ComServer
  {
  protected:
    virtual void SetUp()
    {
      ComServer::SetUp();
      Unknown->QueryInterface(Opc::IID_IOPCServer, Com::ResultPtr(OPCServer));
    }

    virtual void TearDown()
    {
      OPCServer.reset();
      ComServer::TearDown();
    } 

  protected:
    Com::HRESULT AddGroupByName(const char* name, Com::IUnknown** unk)
    {
      bool isActive = true;
      unsigned requestedUpdateRate = 0;
      Opc::OPCHANDLE clientGroupHandle = 0;
      const long* pTimeBias = nullptr;
      const float* pPercentDeadband = nullptr;
      unsigned localeId = 0;

      return OPCServer->AddGroup(name, isActive, requestedUpdateRate, clientGroupHandle, pTimeBias, pPercentDeadband, localeId, unk);
    }
    
  protected:
    std::shared_ptr<Opc::IOPCServer> OPCServer;
  };


}

TEST_F(OpcServer, IOPCServerSupported)
{
  ASSERT_TRUE(static_cast<bool>(OPCServer));
}

TEST_F(OpcServer, AddingGroupWithNullName)
{
  Com::IUnknown::SharedPtr unk;
  const char* nullName = nullptr;
  EXPECT_EQ(AddGroupByName(nullName, Com::ResultPtr(unk)), Com::COM_INVALID_PARAMETER1);
  ASSERT_FALSE(static_cast<bool>(unk));
}

TEST_F(OpcServer, AddingGroupWithEmptyName)
{
  Com::IUnknown::SharedPtr unk;
  const char* emptyName = "";
  EXPECT_EQ(AddGroupByName(emptyName, Com::ResultPtr(unk)), Com::COM_INVALID_PARAMETER1);
  ASSERT_FALSE(static_cast<bool>(unk));
}

TEST_F(OpcServer, AddingGroupByName)
{
  Com::IUnknown::SharedPtr unk;
  const char* groupName = "group";
  EXPECT_EQ(AddGroupByName(groupName, Com::ResultPtr(unk)), Com::COM_SUCCESS);
  ASSERT_TRUE(static_cast<bool>(unk));
}

