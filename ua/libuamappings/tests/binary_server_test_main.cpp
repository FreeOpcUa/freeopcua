/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <chrono>
#include <gtest/gtest.h>
#include <opc/ua/binary_connection.h>
#include <opc/ua/binary_messages.h>
#include <opc/ua/binary_server.h>
#include <opc/ua/binary_stream.h>
#include <thread>

#include "common.h"


int main(int argc, char** argv)
{
  std::unique_ptr<OpcUa::Binary::IncomingConnectionProcessor> clientsProcessor(new EchoProcessor());
  const unsigned short port = 4840;
  std::unique_ptr<OpcUa::Binary::Server> server = OpcUa::Binary::CreateServer(port, std::unique_ptr<OpcUa::Binary::IncomingConnectionProcessor>(clientsProcessor.release()));
  server->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
 
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
