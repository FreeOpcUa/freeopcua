/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef XML_ADDRESS_SPACE_PROCESSOR_H_
#define XML_ADDRESS_SPACE_PROCESSOR_H_

#include <opc/common/interface.h>
#include <libxml/tree.h>

namespace OpcUa
{
namespace Internal
{

class XmlProcessor : private Common::Interface
{
public:
  virtual void Process(xmlNode & node) = 0;
};

} // namespace Internal

} // namespace OpcUa

#endif /* XML_ADDRESS_SPACE_PROCESSOR_H_ */
