/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once

#include <boost/python.hpp>

#include "opc/ua/node.h"
#include "opc/ua/event.h"
#include "opc/ua/subscription.h"

using namespace boost::python;
using namespace OpcUa;

class PySubscriptionHandler: public SubscriptionHandler
{
public:

  PySubscriptionHandler(PyObject * p);
  void DataChange(uint32_t handle, const Node & node, const Variant & val, AttributeId attribute) const override;
  void Event(uint32_t handle, const OpcUa::Event & event) const override;
  void StatusChange(StatusCode status)  const override;
  static void DefaultDataChange(const SubscriptionHandler & self_, uint32_t handle, const Node & node, const object & val, uint32_t attribute);
  static void DefaultEvent(const SubscriptionHandler & self_, uint32_t handle, const OpcUa::Event & event);
  static void DefaultStatusChange(const SubscriptionHandler & self_, StatusCode status);

private:
  PyObject * self;

};

