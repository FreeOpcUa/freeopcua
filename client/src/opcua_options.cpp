/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa client command line options parser.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

#include "opcua_options.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


namespace
{
  namespace po = boost::program_options;
  using namespace OpcUa;

  const char* OPTION_HELP = "help";
  const char* OPTION_GET_ENDPOINTS = "get-endpoints";
  const char* OPTION_BROWSE = "browse";
  const char* OPTION_READ = "read";
  const char* OPTION_WRITE = "write";
  const char* OPTION_CREATE_SUBSCRIPTION = "create-subscription";
  const char* OPTION_FIND_SERVERS = "find-servers";
  const char* OPTION_SERVER_URI = "uri";
  const char* OPTION_NAMESPACE_INDEX = "namespace-index";
  const char* OPTION_ATTRIBUTE = "attribute";

  const char* OPTION_NODE_ID_TWO_BYTE = "node-id-two-byte";
  const char* OPTION_NODE_ID_NUMERIC = "node-id-numeric";
  const char* OPTION_NODE_ID_STRING = "node-id-string";


  const char* OPTION_VALUE_BYTE  = "value-byte"; 
  const char* OPTION_VALUE_SBYTE = "value-sbyte";
  const char* OPTION_VALUE_UINT16 = "value-uint16";
  const char* OPTION_VALUE_INT16 = "value-int16";
  const char* OPTION_VALUE_UINT32 = "value-uint32";
  const char* OPTION_VALUE_INT32 = "value-int32";
  const char* OPTION_VALUE_UINT64 = "value-uint64";
  const char* OPTION_VALUE_INT64 = "value-int64";
  const char* OPTION_VALUE_FLOAT = "value-float";
  const char* OPTION_VALUE_DOUBLE = "value-double";
  const char* OPTION_VALUE_STRING = "value-string";

  uint16_t GetNamespaceIndexOptionValue(const po::variables_map& vm)
  {
    if (vm.count(OPTION_NAMESPACE_INDEX))
    {
      return vm[OPTION_NAMESPACE_INDEX].as<uint16_t>();
    }
    return 0;
  }

  NodeID GetNodeIDOptionValue(const po::variables_map& vm)
  {
    NodeID nodeID;
    if (vm.count(OPTION_NODE_ID_TWO_BYTE))
    {
      nodeID.Encoding = EV_TWO_BYTE;
      nodeID.TwoByteData.Identifier = vm[OPTION_NODE_ID_TWO_BYTE].as<unsigned>();
    }
    else if (vm.count(OPTION_NODE_ID_NUMERIC))
    {
      nodeID.Encoding = EV_NUMERIC;
      nodeID.NumericData.NamespaceIndex = GetNamespaceIndexOptionValue(vm);
      nodeID.NumericData.Identifier = vm[OPTION_NODE_ID_NUMERIC].as<int>();
    }
    else if (vm.count(OPTION_NODE_ID_STRING))
    {
      nodeID.Encoding = EV_STRING;
      nodeID.StringData.NamespaceIndex = GetNamespaceIndexOptionValue(vm);
      nodeID.StringData.Identifier = vm[OPTION_NODE_ID_STRING].as<std::string>();
    }
    else
    {
      nodeID.Encoding = EV_TWO_BYTE;
      nodeID.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::RootFolder);
    }
    return nodeID;
  }

  AttributeID GetAttributeIDOptionValue(const po::variables_map& vm)
  {
    const std::string name = vm[OPTION_ATTRIBUTE].as<std::string>();
    if (name == "node id")
    {
      return AttributeID::NODE_ID;
    }
    if (name == "node class")
    {
      return AttributeID::NODE_ID;
    }
    if (name == "browse name")
    {
      return AttributeID::BROWSE_NAME;
    }
    if (name == "display name")
    {
      return AttributeID::DISPLAY_NAME;
    }
    if (name == "description")
    {
      return AttributeID::DISPLAY_NAME;
    }
    if (name == "write mask")
    {
      return AttributeID::WRITE_MASK;
    }
    if (name == "user write mask")
    {
      return AttributeID::USER_WRITE_MASK;
    }
    if (name == "is abstract")
    {
      return AttributeID::IS_ABSTRACT;
    }
    if (name == "symmetric")
    {
      return AttributeID::SYMMETRIC;
    }
    if (name == "inverse name")
    {
      return AttributeID::INVERSE_NAME;
    }
    if (name == "value")
    {
      return AttributeID::VALUE;
    }
    if (name == "data type")
    {
      return AttributeID::DATA_TYPE;
    }

/*
    CONTAINS_NO_LOOPS = 11,
    EVENT_NOTIFIER = 12,
    DATA_TYPE = 14,
    VALUE_RANK = 15,
    ARRAY_DIMENSIONS = 16,
    ACCESS_LEVEL = 17,
    USER_ACCESS_LEVEL = 18,
    MINIMUM_SAMPLING_INTERVAL = 19,
    HISTORIZING = 20,
    EXECUTABLE = 21,
    USER_EXECUTABLE = 22
*/
    throw std::logic_error(std::string("Unknown attribute: ") + name);
  }

  Variant GetOptionValue(const po::variables_map& vm)
  {
    Variant var;
    if (vm.count(OPTION_VALUE_BYTE))
    {
      var.Value.Byte.push_back(vm[OPTION_VALUE_BYTE].as<uint8_t>());
      return var;
    }
    if (vm.count(OPTION_VALUE_SBYTE))
    {
      var.Value.Byte.push_back(vm[OPTION_VALUE_SBYTE].as<int8_t>());
      return var;
    }
    if (vm.count(OPTION_VALUE_UINT16))
    {
      return vm[OPTION_VALUE_UINT16].as<uint16_t>();
    }
    if (vm.count(OPTION_VALUE_INT16))
    {
      return vm[OPTION_VALUE_INT16].as<int16_t>();
    }
    if (vm.count(OPTION_VALUE_UINT32))
    {
      return vm[OPTION_VALUE_UINT32].as<uint32_t>();
    }
    if (vm.count(OPTION_VALUE_INT32))
    {
      return vm[OPTION_VALUE_INT32].as<int32_t>();
    }
    if (vm.count(OPTION_VALUE_UINT64))
    {
      return vm[OPTION_VALUE_UINT64].as<uint64_t>();
    }
    if (vm.count(OPTION_VALUE_INT64))
    {
      return vm[OPTION_VALUE_INT64].as<int64_t>();
    }
    if (vm.count(OPTION_VALUE_FLOAT))
    {
      return vm[OPTION_VALUE_FLOAT].as<float>();
    }
    if (vm.count(OPTION_VALUE_DOUBLE))
    {
      return vm[OPTION_VALUE_DOUBLE].as<double>();
    }
    if (vm.count(OPTION_VALUE_STRING))
    {
      return vm[OPTION_VALUE_STRING].as<std::string>();
    }
    return Variant();
  } 


 
}


namespace OpcUa
{
  CommandLine::CommandLine(int argc, char** argv)
  {
    // Declare the supported options.
    po::options_description desc("Parameters");
    desc.add_options()
      (OPTION_HELP, "produce help message")
      (OPTION_GET_ENDPOINTS, "List endpoints endpoints.")
      (OPTION_BROWSE, "browse command.")
      (OPTION_READ, "read command.")
      (OPTION_WRITE, "write command.")
      (OPTION_CREATE_SUBSCRIPTION, "create subscription command.")
      (OPTION_FIND_SERVERS, "find servers command.")

      (OPTION_SERVER_URI, po::value<std::string>(), "Uri of the server.")
      (OPTION_ATTRIBUTE, po::value<std::string>(), "Name of attribute.")
      (OPTION_NODE_ID_TWO_BYTE, po::value<unsigned>(), "Two byte NodeId.")
      (OPTION_NODE_ID_NUMERIC, po::value<int>(), "numeric NodeId.")
      (OPTION_NODE_ID_STRING, po::value<std::string>(), "string NodeId.")
      (OPTION_NAMESPACE_INDEX, po::value<uint16_t>(), "Namespace index of the node.")
      (OPTION_VALUE_BYTE, po::value<uint8_t>(), "Byte value.")
      (OPTION_VALUE_SBYTE, po::value<int8_t>(), "Signed byte value.")
      (OPTION_VALUE_UINT16, po::value<uint16_t>(), "UInt16 value.")
      (OPTION_VALUE_INT16, po::value<int16_t>(), "Int16 value.")
      (OPTION_VALUE_UINT32, po::value<uint32_t>(), "UInt32 value.")
      (OPTION_VALUE_INT32, po::value<int32_t>(), "Int32 value.")
      (OPTION_VALUE_UINT64, po::value<uint64_t>(), "UInt64 value.")
      (OPTION_VALUE_INT64, po::value<int64_t>(), "Int64 value.")
      (OPTION_VALUE_FLOAT, po::value<float>(), "Float value.")
      (OPTION_VALUE_DOUBLE, po::value<double>(), "Double value.")
      (OPTION_VALUE_STRING, po::value<std::string>(), "String value.");
;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count(OPTION_HELP)) 
    {
      desc.print(std::cout);
      return;
    }

    ServerURI = vm[OPTION_SERVER_URI].as<std::string>();
    Node = GetNodeIDOptionValue(vm);
    NamespaceIndex = GetNamespaceIndexOptionValue(vm);
    if (vm.count(OPTION_ATTRIBUTE))
    {
      Attribute = GetAttributeIDOptionValue(vm);
    }
    Value = GetOptionValue(vm);
    IsGetEndpoints = vm.count(OPTION_GET_ENDPOINTS);
    IsBrowse = vm.count(OPTION_BROWSE);
    IsRead = vm.count(OPTION_READ);
    IsWrite = vm.count(OPTION_WRITE);
    IsCreateSubscription = vm.count(OPTION_CREATE_SUBSCRIPTION);
    IsFindServers = vm.count(OPTION_FIND_SERVERS);
  }
}
