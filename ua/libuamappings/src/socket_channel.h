/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#include <opc/ua/channel.h>

namespace OpcUa
{

  class SocketChannel : public OpcUa::IOChannel
  {
  public:
    SocketChannel(int sock);
    virtual ~SocketChannel();

    virtual std::size_t Receive(char* data, std::size_t size);
    virtual void Send(const char* message, std::size_t size);

  private:
    int Socket;
  };

}
