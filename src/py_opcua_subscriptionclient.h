/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef WRAP_OPCUA_SUBSCRIPTIONCLIENT_H
#define WRAP_OPCUA_SUBSCRIPTIONCLIENT_H

#include <boost/python.hpp>

#include "opc/ua/node.h"
#include "opc/ua/event.h"
#include "opc/ua/subscription.h"

using namespace boost::python;
using namespace OpcUa;

class PySubscriptionClient: public SubscriptionClient
{
public:

  PySubscriptionClient(PyObject * p);
  void DataChange(uint32_t handle, const Node & node, const Variant & val, AttributeID attribute) const override;
  void Event(uint32_t handle, const OpcUa::Event & event) const override;
  void StatusChange(StatusCode status)  const override;
  static void DefaultDataChange(const SubscriptionClient & self_, uint32_t handle, const Node & node, const object & val, uint32_t attribute);
  static void DefaultEvent(const SubscriptionClient & self_, uint32_t handle, const OpcUa::Event & event);
  static void DefaultStatusChange(const SubscriptionClient & self_, StatusCode status);

private:
  PyObject * self;

};

#endif


