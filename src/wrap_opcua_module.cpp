/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <boost/python.hpp>
#include <boost/python/type_id.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <functional>

#include "opc/ua/client/client.h"
#include "opc/ua/client/binary_server.h"
#include "opc/ua/node.h"
#include "opc/ua/event.h"
#include "opc/ua/server/opcuaserver.h"
#include "opc/ua/protocol/types.h"
#include "opc/ua/services/services.h"
#include "opc/ua/subscription.h"
#include "opc/ua/protocol/string_utils.h"

#include "wrap_opcua_enums.h"
#include "wrap_opcua_helpers.h"

using namespace boost::python;
using namespace OpcUa;


template <typename T>
T Extract(const object & obj)
{
  return extract<T>(obj)();
}

template <typename T>
list ToList(const std::vector<T> objects)
{
  list result;
  std::for_each(objects.begin(), objects.end(),
                [&result](const T & obj)
  {
    result.append(obj);
  }
               );
  return result;
}

template <typename ResultType, typename SourceType>
list ToList(const std::vector<SourceType> objects)
{
  list result;
  std::for_each(objects.begin(), objects.end(),
                [&result](const SourceType & obj)
  {
    result.append(ResultType(obj));
  }
               );
  return result;
}

template <typename T>
std::vector<T> ToVector(const object & list)
{
  std::vector<T> result;
  std::size_t listSize = len(list);

  for (std::size_t i = 0; i < listSize; ++i)
    {
      const object & element = list[i];
      const T & value = Extract<T>(element);
      result.push_back(value);
    }

  return result;
}

struct PyBrowseParameters
{
  unsigned MaxReferenciesCount;
  NodeID NodeToBrowse;
  unsigned Direction;
  NodeID ReferenceTypeID;
  bool IncludeSubtypes;
  unsigned NodeClasses;
  unsigned ResultMask;

  PyBrowseParameters()
    : MaxReferenciesCount(0)
    , Direction(0)
    , IncludeSubtypes(false)
    , NodeClasses(0)
    , ResultMask(0)
  {
  }
};

struct PyReadParameters
{
  double MaxAge;
  TimestampsToReturn TimestampsType;
  list AttributesToRead; // type of elmnts is AttributeValueID
};


struct VariantToPythonObjectConverter
{
  typedef object result_type;

  template <typename T>
  typename std::enable_if<is_container_not_string<T>::value == true, result_type>::type operator()(const T & val)
  {
    return ToList(val);
  }

  template <typename T>
  typename std::enable_if<is_container_not_string<T>::value == false, result_type>::type operator()(const T & val)
  {
    return object(val);
  }
};

object ToObject(const Variant & var)
{
  if (var.IsNul())
    {
      return object();
    }

  return var.Visit(VariantToPythonObjectConverter());
}

Variant ToVariant(const object & object)
{
  Variant var;

  if (extract<std::string>(object).check())
    {
      var = extract<std::string>(object)();
    }

  else if (extract<list>(object).check())
    {
      list plist = (list) object;

      if (len(object) == 0)
        {
        }
      else
        {
          if (extract<int>(object[0]).check())
            {
              var = ToVector<int>(object);
            }

          else if (extract<double>(object[0]).check())
            {
              var = ToVector<double>(object);
            }

          else if (extract<std::string>(object[0]).check())
            {
              var = ToVector<std::string>(object);
            }

          else
            {
              throw std::logic_error("Cannot create variant from python list. Unsupported type.");
            }
        }
    }

  else if (extract<int>(object).check())
    {
      var = Extract<int>(object);
    }

  else if (extract<double>(object).check())
    {
      var = Extract<double>(object);
    }

  else if (extract<NodeID>(object).check())
    {
      var = ToVector<NodeID>(object);
    }

  else
    {
      throw std::logic_error("Cannot create variant from python object. Unsupported type.");
    }

  return var;
}


//similar to ToVariant but gives a hint to what c++ object type the python object should be converted to
Variant ToVariant2(const object & object, VariantType vtype)
{
  Variant var;

  if (extract<list>(object).check())
    {

      list plist = (list) object;

      if (len(object) == 0)
        {
          return var;
        }

      else
        {
          switch (vtype)
            {
            case VariantType::BOOLEAN:
              var = ToVector<bool>(object);
              return var;

            case VariantType::UINT32:
              var = ToVector<uint32_t>(object);
              return var;

            default:
              return ToVariant(object);
            }
        }
    }

  else
    {
      switch (vtype)
        {
        case VariantType::BOOLEAN:
          var = extract<bool>(object)();
          return var;

        case VariantType::UINT16:
        case VariantType::UINT32:
          var = extract<uint32_t>(object)();
          return var;

        default:
          return ToVariant(object);
        }
    }
}

struct PyWriteValue
{
  NodeID Node;
  unsigned Attribute;
  std::string NumericRange;
  DataValue Data;

  PyWriteValue()
    : Attribute(0)
  {
  }

  explicit PyWriteValue(const WriteValue & value)
    : Node(value.Node)
    , Attribute(static_cast<unsigned>(value.Attribute))
    , NumericRange(value.NumericRange)
    , Data(value.Data)
  {
  }
};

WriteValue GetWriteValue(const PyWriteValue & pyValue)
{
  WriteValue result;
  result.Attribute = static_cast<AttributeID>(pyValue.Attribute);
  result.Node = pyValue.Node;
  result.NumericRange = pyValue.NumericRange;

  if (pyValue.Data.Encoding & DATA_VALUE_STATUS_CODE)
    {
      result.Data.Status = static_cast<StatusCode>(pyValue.Data.Status);
      result.Data.Encoding |= DATA_VALUE_STATUS_CODE;
    }

  if (pyValue.Data.Encoding & DATA_VALUE_SERVER_PICOSECONDS)
    {
      result.Data.ServerPicoseconds = pyValue.Data.ServerPicoseconds;
      result.Data.Encoding |= DATA_VALUE_SERVER_PICOSECONDS;
    }

  if (pyValue.Data.Encoding & DATA_VALUE_SOURCE_PICOSECONDS)
    {
      result.Data.SourcePicoseconds = pyValue.Data.SourcePicoseconds;
      result.Data.Encoding |= DATA_VALUE_SOURCE_PICOSECONDS;
    }

  if (pyValue.Data.Encoding & DATA_VALUE_SERVER_TIMESTAMP)
    {
      result.Data.ServerTimestamp.Value = pyValue.Data.ServerTimestamp;
      result.Data.Encoding |= DATA_VALUE_SERVER_TIMESTAMP;
    }

  if (pyValue.Data.Encoding & DATA_VALUE_SOURCE_TIMESTAMP)
    {
      result.Data.SourceTimestamp.Value = pyValue.Data.SourceTimestamp;
      result.Data.Encoding |= DATA_VALUE_SOURCE_TIMESTAMP;
    }

  if (pyValue.Data.Encoding & DATA_VALUE)
    {
      result.Data.Value = pyValue.Data.Value;
      result.Data.Encoding |= DATA_VALUE;
    }

  return result;
}


struct PyVariant
{
  object Value;
  VariantType Type = VariantType::NUL;
  bool IsNull = true;

  PyVariant() = default;

  explicit PyVariant(const Variant & value)
    : Value(ToObject(value))
    , Type(value.Type())
    , IsNull(value.IsNul())
  {
  }
};

class PyServer
{
public:
  explicit PyServer(const std::string & endpointUrl)
    : Impl(CreateBinaryServer(endpointUrl))
  {
  }
  /*
      list FindServers() const
      {
        const FindServersParameters params;
        const std::vector<ApplicationDescription> nativeApps = Impl->Endpoints()->FindServers(params);
        return ToList(nativeApps);
      }

      list GetEndpoints() const
      {
        const EndpointsFilter filter;
        const std::vector<EndpointDescription> endpoints = Impl->Endpoints()->GetEndpoints(filter);
        return ToList(endpoints);
      }
      */
  list Browse(const PyBrowseParameters & p) const
  {
    BrowseDescription description;
    description.NodeToBrowse = p.NodeToBrowse;
    description.ReferenceTypeID = p.ReferenceTypeID;
    description.Direction = static_cast<BrowseDirection>(p.Direction);
    description.IncludeSubtypes = p.IncludeSubtypes;
    description.NodeClasses = p.NodeClasses;
    description.ResultMask = p.ResultMask;

    NodesQuery query;
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = p.MaxReferenciesCount;
    const std::vector<ReferenceDescription> references = Impl->Views()->Browse(query);
    return ToList(references);
  }

  list Read(const PyReadParameters & in)
  {
    ReadParameters params;
    params.MaxAge = in.MaxAge;
    params.TimestampsType = static_cast<TimestampsToReturn>(in.TimestampsType);

    std::size_t listSize = len(in.AttributesToRead);

    for (std::size_t i = 0; i < listSize; ++i)
      {
        const AttributeValueID & value = extract<AttributeValueID>(in.AttributesToRead[i]); // XXX check
        AttributeValueID attr;
        attr.Attribute = value.Attribute;
        attr.DataEncoding.NamespaceIndex = value.DataEncoding.NamespaceIndex;
        attr.DataEncoding.Name = value.DataEncoding.Name;
        attr.IndexRange = value.IndexRange;
        attr.Node = value.Node;
        params.AttributesToRead.push_back(attr);
      }

    std::vector<DataValue> data = Impl->Attributes()->Read(params);
    return ToList<DataValue, DataValue>(data); // XXX std::vector<DataValue> has been wrapped.
  }

  //    std::vector<StatusCode> Write(const std::vector<WriteValue>& filter) = 0;
  list Write(const list & in)
  {
    const std::vector<PyWriteValue> & pyValues = ToVector<PyWriteValue>(in);
    std::vector<WriteValue> values;

    for (std::vector<PyWriteValue>::const_iterator valueIt = pyValues.begin(); valueIt != pyValues.end(); ++valueIt)
      {
        const PyWriteValue & pyValue = *valueIt;
        const WriteValue & value = GetWriteValue(pyValue);
        values.push_back(value);
      }

    const list & result = ToList<unsigned, StatusCode>(Impl->Attributes()->Write(values));
    return result;
  }

private:
  Services::SharedPtr Impl;
};

/*
      StructureStatusResult
      MessageSecurityMode
      StructureApplicationDescription
      SecurityTokenRequestType
      StructureUserIdentifyToken
      AnonymousIdentifyToken
      UserNameIdentifyToken
      X509IdentifyToken
      StructureBuildInfo
      SoftwareCertificate
      StructureSignedSoftwareCertificate
      StructureAddNodesItem
      StructureAddReferencesItem
      StructureDeleteNodesItem
      StructureDeleteReferencesItem
      RedundancySupport
      ServerState
      StructureSamplingIntervalDiagnosticsDataType
      StructureServerDiagnosticsSummaryType
      StructureServerStatusDataType
      StructureSessionDiagnosticsDataType
      StructureSessionSecurityDiagnosticsDataType
      StructureServiceCounterDataType
      StructureSubscriptionDiagnosticsDataType
      StructureModelChangeStructureDataType
      StructureRange
      StructureEUInformation
      StructureSemanticChangeStructureDataType
      ImageBmp
      ImageGif
      ImageJpg
      ImagePng
      ServerType
      ServerArray
      NamespaceArray
      ServerStatus
      ServiceLevel
      ServerCapabilities
      ServerDiagnostics
      VendorServerInfo
      ServerRedundancy
      ServerCapabilitiesType
      ServerProfileArray
      LocaleIDArray
      MinSupportedSampleRate
      ModellingRules
      ServerDiagnosticsType
      ServerDiagnosticsSummary
      SamplingIntervalDiagnosticsArray
      SubscriptionDiagnosticsArray
      EnableFlag
      SessionDiagnosticsSummaryType
      SessionDiagnosticsObjectType
      VendorServerInfoType
      ServerRedundancyType
      RedundancySupportTypeRedundancySupport
      BaseEventType
      EventID
      EventType
      SourceNode
      SourceName
      Time
      ReceiveTime
      Message
      Severity
      SystemEventType
      DeviceFailureEventType
      BaseModelChangeEventType
      ServerVendorCapabilityType
      ServerStatusType
      ServerDiagnosticsSummaryType
      SamplingIntervalDiagnosticsArrayType
      SamplingIntervalDiagnosticsType
      SubscriptionDiagnosticsArrayType
      SubscriptionDiagnosticsType
      SessionsDiagnosticsArrayType
      ServerDiagnosticsVariableType
      SessionSecurityDiagnosticsArrayType
      SessionSecurityDiagnosticsType
      StateType
      StateNumber
      TransitionType
      HistoricalEventConfigurationType
      MaxBrowseContinuationPoints
      MaxHistoryContinuationPoints
      SemanticChangeEventType
      Auditing
      SessionsDiagnosticsSummary
      AggregateFunctions
      RefreshStartEventType
      RefreshEndEventType
      RefreshRequiredEventType
      EventTypes
      SoftwareCertificates
      AlwaysGeneratesEvent
      StartTime
      CurrentTime
      State
      BuildInfo
      ProductURI
      ManufacturerName
      ProductName
      SoftwareVersion
      BuildNumber
      BuildDate
      SecondsTillShutdown
      ShutdownReason
      LocalTime
      IdTypeEnumStrings
      MessageSecurityModeEnumStrings
      SecurityTokenRequestTypeEnumStrings
      RedundancySupportEnumStrings
      ServerStateEnumStrings
      HasTrueSubState
      HasFalseSubState
      HasCondition
*/

class PyNode: public Node
{
public:
  PyNode(Services::SharedPtr srv, const NodeID & id)
    : Node(srv, id)
  {
  }

  PyNode(const Node & other)
    : Node(other.GetServices(), other.GetId(), other.GetName())
  {
  }

  object PyGetValue()
  {
    return ToObject(Node::GetValue());
  }

  object PyGetName()
  {
    return ToObject(Node::GetName());
  }

  NodeID PyGetNodeID()
  {
    return GetId();
  }

  object PySetValue(const object & val, VariantType hint = VariantType::NUL, DateTime t = CurrentDateTime())
  {
    Variant var = ToVariant2(val, hint);
    StatusCode code = Node::SetValue(var, t);
    return ToObject(code);
  }

  object PySetDataValue(const DataValue & dval)
  {
    StatusCode code = Node::SetValue(dval);
    return ToObject(code);
  }

  list PyGetChildren()
  {
    list result;

    for (Node n : Node::GetChildren())
      {
        result.append(PyNode(n));
      }

    return result;
  }

  PyNode PyGetChild(const object & path)
  {
    if (extract<std::string>(path).check())
      {
        Node n = Node::GetChild(extract<std::string>(path)());
        return PyNode(n);
      }

    else
      {
        Node n = Node::GetChild(ToVector<std::string>(path));
        return PyNode(n);
      }
  }

  PyNode PyAddFolder(const std::string & browsename)
  {
    return PyNode(Node::AddFolder(browsename));
  }

  PyNode PyAddFolder2(const std::string & nodeid, const std::string & browsename)
  {
    return PyNode(Node::AddFolder(nodeid, browsename));
  }

  PyNode PyAddFolder3(const NodeID & nodeid, const QualifiedName browsename)
  {
    return PyNode(Node::AddFolder(nodeid, browsename));
  }

  PyNode PyAddObject(const std::string & browsename)
  {
    return PyNode(Node::AddObject(browsename));
  }

  PyNode PyAddObject2(const std::string & nodeid, const std::string & browsename)
  {
    return PyNode(Node::AddObject(ToNodeID(nodeid), ToQualifiedName(browsename, 0)));
  }

  PyNode PyAddObject3(const NodeID & nodeid, const QualifiedName & browsename)
  {
    return PyNode(Node::AddObject(nodeid, browsename));
  }

  PyNode PyAddVariable(const std::string & browsename, const object & val)
  {
    return PyNode(Node::AddVariable(browsename, ToVariant(val)));
  }

  PyNode PyAddVariable2(const std::string & nodeid, const std::string & browsename, const object & val)
  {
    return PyNode(Node::AddVariable(nodeid, browsename, ToVariant(val)));
  }

  PyNode PyAddVariable3(const NodeID & nodeid, const QualifiedName & browsename, const object & val)
  {
    return PyNode(Node::AddVariable(nodeid, browsename, ToVariant(val)));
  }

  PyNode PyAddProperty(const std::string & browsename, const object & val)
  {
    return PyNode(Node::AddProperty(browsename, ToVariant(val)));
  }

  PyNode PyAddProperty2(const std::string & nodeid, const std::string & browsename, const object & val)
  {
    return PyNode(Node::AddProperty(nodeid, browsename, ToVariant(val)));
  }

  PyNode PyAddProperty3(const NodeID & nodeid, const QualifiedName & browsename, const object & val)
  {
    return PyNode(Node::AddProperty(nodeid, browsename, ToVariant(val)));
  }
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PyNodeSetValue_stubs, PyNode::PySetValue, 1, 3);

class PyEvent : public Event
{
public:
  using Event::Event;
  PyEvent(const Event & other) : Event(other) {}

  void PySetValue(const std::string & name, const object & val)
  {
    SetValue(name, ToVariant(val));
  }

  object PyGetValue(const std::string & name)
  {
    return ToObject(GetValue(name));
  }

  void SetMessage(const std::string & name)
  {
    Message = LocalizedText(name);
  }

  object GetMessage()
  {
    std::cout << "Getting message: " << Message.Text << std::endl;
    return ToObject(Message.Text);
  }
  void SetSourceNode(NodeID node)
  {
    SourceNode = node;
  }

  NodeID GetSourceNode()
  {
    return SourceNode;
  }

  object GetTime()
  {
    return ToObject(Time);
  };

  void SetTime(object datetime)
  {
    //FIXME: implement
  }

};


std::string parse_python_exception()
{
  PyObject * type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
  // Fetch the exception info from the Python C API
  PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
  // Fallback error
  std::string ret("Unfetchable Python error");

  // If the fetch got a type pointer, parse the type into the exception string
  if (type_ptr != NULL)
    {
      handle<> h_type(type_ptr);
      str type_pstr(h_type);
      // Extract the string from the boost::python object
      extract<std::string> e_type_pstr(type_pstr);

      // If a valid string extraction is available, use it
      // otherwise use fallback
      if (e_type_pstr.check())
        { ret = e_type_pstr(); }

      else
        { ret = "Unknown exception type"; }
    }

  // Do the same for the exception value (the stringification of the exception)
  if (value_ptr != NULL)
    {
      handle<> h_val(value_ptr);
      str a(h_val);
      extract<std::string> returned(a);

      if (returned.check())
        { ret += ": " + returned(); }

      else
        { ret += std::string(": Unparseable Python error: "); }
    }

  // Parse lines from the traceback using the Python traceback module
  if (traceback_ptr != NULL)
    {
      handle<> h_tb(traceback_ptr);
      // Load the traceback module and the format_tb function
      object tb(import("traceback"));
      object fmt_tb(tb.attr("format_tb"));
      // Call format_tb to get a list of traceback strings
      object tb_list(fmt_tb(h_tb));
      // Join the traceback strings into a single string
      object tb_str(str("\n").join(tb_list));
      // Extract the string, check the extraction, and fallback in necessary
      extract<std::string> returned(tb_str);

      if (returned.check())
        { ret += ": " + returned(); }

      else
        { ret += std::string(": Unparseable Python traceback"); }
    }

  return ret;
}

class PySubscriptionClient: public SubscriptionClient
{
public:
  PySubscriptionClient(PyObject * p) : self(p) {}
  //PySubscriptionClient(PyObject *p, const SubscriptionClient& x)  : SubscriptionClient(x), self(p) {} //copy construct

  void DataChange(uint32_t handle, const Node & node, const Variant & val, AttributeID attribute) const override
  {
    PyGILState_STATE state = PyGILState_Ensure();

    try
      {
        call_method<void>(self, "data_change", handle, PyNode(node), ToObject(val) , (uint32_t) attribute);
      }

    catch (const error_already_set & ex)
      {
        std::string perror_str = parse_python_exception();
        std::cout << "Error in Python: " << perror_str << std::endl;
      }

    PyGILState_Release(state);
  };

  static void DefaultDataChange(const SubscriptionClient & self_, uint32_t handle, const PyNode & node, const object & val, uint32_t attribute)
  {
    std::cout << "No callback defined in python for DataChange" << std::endl;
  }

  void Event(uint32_t handle, const OpcUa::Event & event) const override
  {
    PyGILState_STATE state = PyGILState_Ensure();

    try
      {
        call_method<void>(self, "event", handle, PyEvent(event));
      }

    catch (const error_already_set & ex)
      {
        std::string perror_str = parse_python_exception();
        std::cout << "Error in Python: " << perror_str << std::endl;
      }

    PyGILState_Release(state);
  };

  static void DefaultEvent(const SubscriptionClient & self_, uint32_t handle, const PyEvent & event)
  {
    std::cout << "No callback defined in python for Event" << std::endl;
  }

  void StatusChange(StatusCode status)  const override
  {
    PyGILState_STATE state = PyGILState_Ensure();

    try
      {
        call_method<void>(self, "status_change", status);
      }

    catch (const error_already_set & ex)
      {
        std::string perror_str = parse_python_exception();
        std::cout << "Error in Python: " << perror_str << std::endl;
      }

    PyGILState_Release(state);
  };

  static void DefaultStatusChange(const SubscriptionClient & self_, StatusCode status)
  {
    std::cout << "No callback defined in python for StatusChange" << std::endl;
  }




private:
  PyObject * const self;
};

class PySubscription
{
public:
  PySubscription(std::shared_ptr<Subscription> other): Sub(other) { }  //converting to shared pointer, should be ok
  PySubscription() { throw std::runtime_error("Subscription cannot be instanciated from Python"); }
  void Delete() { Sub->Delete(); }
  uint32_t SubscribeDataChange(PyNode node) { return Sub->SubscribeDataChange(node, AttributeID::VALUE); }
  uint32_t SubscribeDataChange2(PyNode node, AttributeID attr) { return Sub->SubscribeDataChange(node, attr); }
  void UnSubscribe(uint32_t id) { return Sub->UnSubscribe(id); }
  uint32_t SubscribeEvents() { return Sub->SubscribeEvents(); }
  uint32_t SubscribeEvents2(const Node node, const Node & eventtype) { return Sub->SubscribeEvents(node, eventtype); }

private:
  std::shared_ptr<Subscription> Sub;
};

class PyClient: public RemoteClient
{
public:
  using RemoteClient::RemoteClient;

  PyNode PyGetRootNode()
  {
    return PyNode(Server, ObjectID::RootFolder);
  }

  PyNode PyGetObjectsNode()
  {
    return PyNode(Server, ObjectID::ObjectsFolder);
  }

  PyNode PyGetServerNode()
  {
    return PyNode(Server, ObjectID::Server);
  }

  PyNode PyGetNode(NodeID nodeid)
  {
    return PyNode(RemoteClient::GetNode(nodeid));
  }

  //PyNode PyGetNodeFromPath(const object& path) { return Client::Client::GetNodeFromPath(ToVector<std::string>(path)); }
  PySubscription CreateSubscription(uint period, PySubscriptionClient & callback)
  {
    return PySubscription(RemoteClient::CreateSubscription(period, callback));
  }
};

class PyOPCUAServer: public OPCUAServer
{
public:
  using OPCUAServer::OPCUAServer;

  PyNode PyGetRootNode() const
  {
    return PyNode(Registry->GetServer(), ObjectID::RootFolder);
  }

  PyNode PyGetObjectsNode() const
  {
    return PyNode(Registry->GetServer(), ObjectID::ObjectsFolder);
  }

  PyNode PyGetServerNode() const
  {
    return PyNode(Registry->GetServer(), ObjectID::Server);
  }

  PyNode PyGetNode(const NodeID & nodeid) const
  {
    return PyNode(OPCUAServer::GetNode(nodeid));
  }

  PyNode PyGetNodeFromPath(const object & path) const
  {
    return OPCUAServer::GetNodeFromPath(ToVector<std::string>(path));
  }

  PySubscription CreateSubscription(uint period, PySubscriptionClient & callback)
  {
    return PySubscription(OPCUAServer::CreateSubscription(period, callback));
  }
};

//--------------------------------------------------------------------------
// NodeID helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<NodeID> NodeID_constructor(const std::string & encodedNodeID)
{
  return boost::shared_ptr<NodeID>(new NodeID(ToNodeID(encodedNodeID)));
}

static object NodeID_GetIdentifier(const NodeID & self)
{
  if (self.IsInteger())
    {
      return object(self.GetIntegerIdentifier());
    }

  else if (self.IsString())
    {
      return object(self.GetStringIdentifier());
    }

  else if (self.IsGuid())
    {
      return object(self.GetGuidIdentifier());
    }

  else if (self.IsBinary())
    {
      return object(self.GetBinaryIdentifier());
    }

  else
    {
      throw std::logic_error("Error unknown identifier.");
    }
}

//--------------------------------------------------------------------------
// DataValue helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<DataValue> DataValue_constructor1(const object & obj)
{
  return boost::shared_ptr<DataValue>(new DataValue(ToVariant(obj)));
}

static boost::shared_ptr<DataValue> DataValue_constructor2(const object & obj, VariantType vtype)
{
  return boost::shared_ptr<DataValue>(new DataValue(ToVariant2(obj, vtype)));
}

static object  DataValue_get_value(const DataValue & self)
{ return ToObject(self.Value); }

static void DataValue_set_value(DataValue & self, const object & obj, VariantType vtype)
{ self.Value = ToVariant2(obj, vtype); self.Encoding |= DATA_VALUE; }

static StatusCode DataValue_get_status(const DataValue & self)
{ return self.Status; }

static void DataValue_set_status(DataValue & self, const StatusCode & sc)
{ self.Status = sc; self.Encoding |= DATA_VALUE_STATUS_CODE; }

static DateTime DataValue_get_source_timestamp(const DataValue & self)
{ return self.SourceTimestamp; }

static void DataValue_set_source_timestamp(DataValue & self, const DateTime & dt)
{ self.SourceTimestamp = dt; self.Encoding |= DATA_VALUE_SOURCE_TIMESTAMP; }

static uint16_t DataValue_get_source_picoseconds(const DataValue & self)
{ return self.SourcePicoseconds; }

static void DataValue_set_source_picoseconds(DataValue & self, uint16_t ps)
{ self.SourcePicoseconds = ps; self.Encoding |= DATA_VALUE_SOURCE_PICOSECONDS; }

static DateTime DataValue_get_server_timestamp(const DataValue & self)
{ return self.ServerTimestamp; }

static void DataValue_set_server_timestamp(DataValue & self, const DateTime & dt)
{ self.ServerTimestamp = dt; self.Encoding |= DATA_VALUE_SERVER_TIMESTAMP; }

static uint16_t DataValue_get_server_picoseconds(const DataValue & self)
{ return self.ServerPicoseconds; }

static void DataValue_set_server_picoseconds(DataValue & self, uint16_t ps)
{ self.ServerPicoseconds = ps; self.Encoding |= DATA_VALUE_SERVER_PICOSECONDS; }

//--------------------------------------------------------------------------
// module
//--------------------------------------------------------------------------

BOOST_PYTHON_MODULE(opcua)
{

  using self_ns::str; // hack to enable __str__ in python classes with str(self)

  PyEval_InitThreads();

  wrap_opcua_enums();

  to_python_converter<std::vector<std::string>, vector_to_python_converter<std::string>>();
  vector_from_python_converter<std::string>();

  class_<DateTime>("DateTime", init<>())
  .def(init<int64_t>())
  ;

  def("CurrentDateTime", &CurrentDateTime);
  def("ToDateTime", &ToDateTime);
  def("ToTimeT", &ToTimeT);

  class_<LocalizedText>("LocalizedText")
  .def_readwrite("Encoding", &LocalizedText::Encoding)
  .def_readwrite("Locale", &LocalizedText::Locale)
  .def_readwrite("Text", &LocalizedText::Text)
  ;

  class_<NodeID, boost::shared_ptr<NodeID>>("NodeID")
  .def(init<uint32_t, uint16_t>())
  .def(init<std::string, uint16_t>())
  .def("__init__", make_constructor(NodeID_constructor)) // XXX add this constructor to freeopcua
  .add_property("namespace_index", &NodeID::GetNamespaceIndex)
  .add_property("identifier", &NodeID_GetIdentifier)
  .add_property("encoding", &NodeID::GetEncodingValue)
  .add_property("is_integer", &NodeID::IsInteger)
  .add_property("is_binary", &NodeID::IsBinary)
  .add_property("is_guid", &NodeID::IsGuid)
  .add_property("is_string", &NodeID::IsString)
  .def_readonly("namespace_uri", &NodeID::NamespaceURI)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  class_<QualifiedName>("QualifiedName")
  .def(init<uint16_t, std::string>())
  .def(init<std::string, uint16_t>()) // XXX ah, right
  //.def("parse", &ToQualifiedName)      XXX could be def(), dropped it's mostly useless
  .def_readwrite("namespace_index", &QualifiedName::NamespaceIndex)
  .def_readwrite("name", &QualifiedName::Name)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  class_<DataValue, boost::shared_ptr<DataValue>>("DataValue")
  .def("__init__", make_constructor(DataValue_constructor1))  // XXX Variant vs object
  .def("__init__", make_constructor(DataValue_constructor2)) // XXX Variant,VariantType vs object,VariantType
#define _property(X) add_property( #X, &DataValue_get_ ## X, &DataValue_set_ ## X)
  ._property(value)
  ._property(status)
  ._property(source_timestamp)
  ._property(source_picoseconds)
  ._property(server_timestamp)
  ._property(server_picoseconds)
#undef _property
  ;

  to_python_converter<std::vector<DataValue>, vector_to_python_converter<DataValue>>();

  class_<ApplicationDescription>("ApplicationDescription")
  .def_readwrite("uri", &ApplicationDescription::URI)
  .def_readwrite("product_uri", &ApplicationDescription::ProductURI)
  .def_readwrite("name", &ApplicationDescription::Name)
  .def_readwrite("type", &ApplicationDescription::Type)
  .def_readwrite("gateway_server_uri", &ApplicationDescription::GatewayServerURI)
  .def_readwrite("discovery_profile_uri", &ApplicationDescription::DiscoveryProfileURI)
  //.def_readwrite("discovery_urls", &ApplicationDescription::DiscoveryURLs) XXX
  .add_vector_property("discovery_urls", ApplicationDescription, std::string, DiscoveryURLs)
  ;

  to_python_converter<std::vector<ApplicationDescription>, vector_to_python_converter<ApplicationDescription>>();

  class_<UserTokenPolicy>("UserTokenPolicy")
  .def_readwrite("policy_id", &UserTokenPolicy::PolicyID)
  .def_readwrite("token_type", &UserTokenPolicy::TokenType)
  .def_readwrite("issued_token_type", &UserTokenPolicy::IssuedTokenType)
  .def_readwrite("issuer_endpoint_url", &UserTokenPolicy::IssuerEndpointURL)
  .def_readwrite("security_policy_uri", &UserTokenPolicy::SecurityPolicyURI)
  ;

  to_python_converter<std::vector<UserTokenPolicy>, vector_to_python_converter<UserTokenPolicy>>();
  vector_from_python_converter<UserTokenPolicy>();

  class_<EndpointDescription>("EndpointDescription")
  .def_readwrite("url", &EndpointDescription::EndpointURL)
  .def_readwrite("server_description", &EndpointDescription::ServerDescription)
  //.def_readwrite("certificate", &EndpointDescription::ServerCertificate)
  .def_readwrite("security_mode", &EndpointDescription::SecurityMode)
  .def_readwrite("security_policy_uri", &EndpointDescription::SecurityPolicyURI)
  //.def_readwrite("user_identify_tokens", &EndpointDescription::UserIdentifyTokens) XXX
  .add_vector_property("user_identify_tokens", EndpointDescription, UserTokenPolicy, UserIdentifyTokens)
  .def_readwrite("transport_profile_uri", &EndpointDescription::TransportProfileURI)
  .def_readwrite("security_level", &EndpointDescription::SecurityLevel)
  ;

  to_python_converter<std::vector<EndpointDescription>, vector_to_python_converter<EndpointDescription>>();

  // XXX delete
  class_<PyBrowseParameters>("BrowseParameters")
  .def_readwrite("max_referencies_count", &PyBrowseParameters::MaxReferenciesCount)
  .def_readwrite("node_to_browse", &PyBrowseParameters::NodeToBrowse)
  .def_readwrite("direction", &PyBrowseParameters::Direction)
  .def_readwrite("reference_type_id", &PyBrowseParameters::ReferenceTypeID)
  .def_readwrite("include_subtypes", &PyBrowseParameters::IncludeSubtypes)
  .def_readwrite("node_classes", &PyBrowseParameters::NodeClasses)
  .def_readwrite("result_mask", &PyBrowseParameters::ResultMask);

  class_<ReferenceDescription>("ReferenceDescription")
  .def_readwrite("reference_type_id", &ReferenceDescription::ReferenceTypeID)
  .def_readwrite("is_forward", &ReferenceDescription::IsForward)
  .def_readwrite("target_node_id", &ReferenceDescription::TargetNodeID)
  .def_readwrite("browse_name", &ReferenceDescription::BrowseName)
  .def_readwrite("display_name", &ReferenceDescription::DisplayName)
  .def_readwrite("target_node_class", &ReferenceDescription::TargetNodeClass)
  .def_readwrite("target_node_type_definition", &ReferenceDescription::TargetNodeTypeDefinition)
  ;

  to_python_converter<std::vector<ReferenceDescription>, vector_to_python_converter<ReferenceDescription>>();
  
  // XXX delete
  class_<PyReadParameters>("ReadParameters")
  .def_readwrite("max_age", &PyReadParameters::MaxAge)
  .def_readwrite("timestamps_to_return", &PyReadParameters::TimestampsType)
  .def_readwrite("attributes_to_read", &PyReadParameters::AttributesToRead);

  class_<AttributeValueID>("AttributeValueID")
  .def_readwrite("node", &AttributeValueID::Node)
  .def_readwrite("attribute", &AttributeValueID::Attribute)
  .def_readwrite("index_range", &AttributeValueID::IndexRange)
  .def_readwrite("data_encoding", &AttributeValueID::DataEncoding)
  ;

  class_<PyWriteValue>("WriteValue", "Parameters data for writing.")
  .def_readwrite("node", &PyWriteValue::Node)
  .def_readwrite("attribute", &PyWriteValue::Attribute)
  .def_readwrite("numeric_range", &PyWriteValue::NumericRange)
  .def_readwrite("data", &PyWriteValue::Data);

  class_<PyVariant>("Variant")
  .def_readonly("value", &PyVariant::Value)
  .def_readonly("type", &PyVariant::Type)
  .def_readonly("is_null", &PyVariant::IsNull)
  ;

  class_<PyNode>("Node", init<Services::SharedPtr, NodeID>())
  .def(init<Node>())
  .def("get_id", &PyNode::PyGetNodeID)
  .def("get_attribute", &PyNode::GetAttribute)
  .def("set_attribute", &PyNode::SetAttribute)
  .def("get_value", &PyNode::PyGetValue)
  .def("set_value", &PyNode::PySetValue, PyNodeSetValue_stubs((arg("value"), arg("hint") = VariantType::NUL, arg("DateTime") = CurrentDateTime()), "set a node value."))
  .def("set_value", &PyNode::PySetDataValue)
  .def("get_properties", &PyNode::GetProperties)
  .def("get_variables", &PyNode::GetVariables)
  .def("get_name", &PyNode::PyGetName)
  .def("get_children", &PyNode::PyGetChildren)
  .def("get_child", &PyNode::PyGetChild)
  .def("add_folder", &PyNode::PyAddFolder)
  .def("add_folder", &PyNode::PyAddFolder2)
  .def("add_object", &PyNode::PyAddObject)
  .def("add_object", &PyNode::PyAddObject2)
  .def("add_variable", &PyNode::PyAddVariable)
  .def("add_variable", &PyNode::PyAddVariable2)
  .def("add_variable", &PyNode::PyAddVariable3)
  .def("add_property", &PyNode::PyAddProperty)
  .def("add_property", &PyNode::PyAddProperty2)
  .def("add_property", &PyNode::PyAddProperty3)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  class_<std::vector<Node> >("NodeVector")
  .def(vector_indexing_suite<std::vector<Node> >())
  ;

  class_<SubscriptionClient, PySubscriptionClient, boost::noncopyable>("SubscriptionClient", init<>())
  .def("data_change", &PySubscriptionClient::DefaultDataChange)
  .def("event", &PySubscriptionClient::DefaultEvent)
  .def("status_change", &PySubscriptionClient::DefaultStatusChange)
  ;


  class_<PyEvent>("Event", init<const NodeID &>())
  .def("get_value", &PyEvent::PyGetValue)
  .def("set_value", &PyEvent::PySetValue)
  .def_readwrite("event_id", &PyEvent::EventId)
  .def_readwrite("event_type", &PyEvent::EventType)
  .def_readwrite("local_time", &PyEvent::LocalTime)
  .def_readwrite("receive_time", &PyEvent::ReceiveTime)
  .def_readwrite("time", &PyEvent::Time)
  .def_readwrite("source_name", &PyEvent::SourceName)
  .add_property("message2", &PyEvent::GetMessage, &PyEvent::SetMessage)
  .def_readwrite("severity", &PyEvent::Severity)
  .add_property("message", &PyEvent::GetMessage, &PyEvent::SetMessage)
  .add_property("source_node", &PyEvent::GetSourceNode, &PyEvent::SetSourceNode)
  .add_property("time", &PyEvent::GetTime, &PyEvent::SetTime)
  ;

  class_<PySubscription>("Subscription", init<std::shared_ptr<Subscription>>())
  .def("subscribe_data_change", &PySubscription::SubscribeDataChange)
  .def("subscribe_data_change", &PySubscription::SubscribeDataChange2)
  .def("delete", &PySubscription::Delete)
  .def("unsubscribe", &PySubscription::UnSubscribe)
  .def("subscribe_events", &PySubscription::SubscribeEvents)
  .def("subscribe_events", &PySubscription::SubscribeEvents2)
  ;

  class_<PyClient, boost::noncopyable>("Client", init<>())
  .def(init<bool>())
  .def("connect", &PyClient::Connect)
  .def("disconnect", &PyClient::Disconnect)
  .def("get_root_node", &PyClient::PyGetRootNode)
  .def("get_objects_node", &PyClient::PyGetObjectsNode)
  .def("get_server_node", &PyClient::PyGetServerNode)
  .def("get_node", &PyClient::PyGetNode)
  .def("set_endpoint", &PyClient::SetEndpoint)
  .def("get_endpoint", &PyClient::GetEndpoint)
  .def("set_session_name", &PyClient::SetSessionName)
  .def("get_session_name", &PyClient::GetSessionName)
  .def("get_uri", &PyClient::GetURI)
  .def("set_uri", &PyClient::SetURI)
  .def("set_security_policy", &PyClient::SetSecurityPolicy)
  .def("get_security_policy", &PyClient::GetSecurityPolicy)
  .def("create_subscription", &PyClient::CreateSubscription)
  ;

  class_<PyOPCUAServer, boost::noncopyable >("Server", init<>())
  .def(init<bool>())
  .def("start", &PyOPCUAServer::Start)
  .def("stop", &PyOPCUAServer::Stop)
  .def("get_root_node", &PyOPCUAServer::PyGetRootNode)
  .def("get_objects_node", &PyOPCUAServer::PyGetObjectsNode)
  .def("get_server_node", &PyOPCUAServer::PyGetServerNode)
  .def("get_node", &PyOPCUAServer::PyGetNode)
  //.def("get_node_from_path", &PyOPCUAServer::PyGetNodeFromPath)
  //.def("get_node_from_qn_path", NodeFromPathQN)
  .def("set_uri", &PyOPCUAServer::SetServerURI)
  .def("add_xml_address_space", &PyOPCUAServer::AddAddressSpace)
  .def("set_server_name", &PyOPCUAServer::SetServerName)
  .def("set_endpoint", &PyOPCUAServer::SetEndpoint)
  .def("load_cpp_addressspace", &PyOPCUAServer::SetLoadCppAddressSpace)
  .def("create_subscription", &PyOPCUAServer::CreateSubscription)
  ;

}

