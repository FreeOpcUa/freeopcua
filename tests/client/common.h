/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_TESTS_COMMON_H__
#define __OPC_UA_TESTS_COMMON_H__

#include <opc/common/uri_facade.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdlib.h>


inline std::string GetHost()
{
  if (const char* uri = getenv("TEST_SERVER_URI"))
  {
    return OpcUa::Internal::Uri(uri).Host();
  }
  return "localhost";
}

inline int GetPort()
{
  if (const char* uri = getenv("TEST_SERVER_URI"))
  {
    return OpcUa::Internal::Uri(uri).Port();
  }
  return 4841;
}

inline std::string GetEndpoint()
{
  if (char* endpoint = getenv("TEST_SERVER_URI"))
  {
    return endpoint;
  }
  return "opc.tcp://localhost:4841";
}


#endif // __OPC_UA_TESTS_COMMON_H__

