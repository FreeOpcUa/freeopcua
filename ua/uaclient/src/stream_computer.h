/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoints.h"

#include <opc/ua/channel.h>
#include <opc/ua/client/computer.h>

namespace OpcUa
{
  namespace Internal
  {

    template <typename StreamType>
    class Computer : public OpcUa::Remote::Computer
    {
    public:
      explicit Computer(std::shared_ptr<IOChannel> channel)
        : Channel(channel)
        , Stream(channel)
      {
      }

      virtual void CreateSession(const Remote::SessionParameters& parameters)
      {
      }

      virtual void UpdateSession(const Remote::IdentifyParameters& parameters)
      {
      }

      virtual void CloseSession()
      {
      }

      virtual std::shared_ptr<Remote::EndpointServices> Endpoints() const
      {
        return std::shared_ptr<Remote::EndpointServices>(new Internal::EndpointServices<StreamType>(Channel));
      }

      virtual std::shared_ptr<Remote::ViewServices> Views() const
      {
        return std::shared_ptr<Remote::ViewServices>();
      }

      virtual std::shared_ptr<Remote::AttributeServices> Attributes() const
      {
        return std::shared_ptr<Remote::AttributeServices>();
      }

    private:
      std::shared_ptr<IOChannel> Channel;
      StreamType Stream;
    };

  } // namespace Internal
} // namespace OpcUa

