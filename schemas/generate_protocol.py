"""
Generate address space c++ code from xml file specification
"""
import sys
from copy import copy

import xml.etree.ElementTree as ET

from IPython import embed

import generate_model as gm


#declare default constructor in header, then constructor must be implemented
NeedConstructor = ["RelativePathElement", "OpenSecureChannelParameters", "UserIdentityToken", "RequestHeader", "ResponseHeader", "ReadParameters", "UserIdentityToken", "BrowseDescription", "ReferenceDescription", "CreateSubscriptionParameters", "PublishResult", "NotificationMessage", "SetPublishingModeParameters"]
IgnoredEnums = ["IdType", "NodeIdType"]
#by default we split requests and respons in header and parameters, but some are so simple we do not split them
NoSplitStruct = ["GetEndpointsResponse", "CloseSessionRequest", "AddNodesResponse", "BrowseResponse", "HistoryReadResponse", "HistoryUpdateResponse", "RegisterServerResponse", "CloseSecureChannelRequest", "CloseSecureChannelResponse", "CloseSessionRequest", "CloseSessionResponse", "UnregisterNodesResponse", "MonitoredItemModifyRequest", "MonitoredItemsCreateRequest"]
OverrideTypes = {"AttributeId": "AttributeId",  "ResultMask": "BrowseResultMask", "NodeClassMask": "NodeClass", "AccessLevel": "VariableAccessLevel", "UserAccessLevel": "VariableAccessLevel", "NotificationData": "NotificationData"}
OverrideNames = {"RequestHeader": "Header", "ResponseHeader": "Header", "StatusCode": "Status", "NodesToRead": "AttributesToRead"} # "MonitoringMode": "Mode",, "NotificationMessage": "Notification", "NodeIdType": "Type"}

#list of UA structure we want to enable, some structures may
#need to be added by hand (those generated from splitted request and response structs)
EnabledStructs = [\
    #we want to implement som struct by hand, to make better interface or simply because they are too complicated 
    #'NodeId',
    #'TwoByteNodeId',
    #'FourByteNodeId',
    #'NumericNodeId',
    #'StringNodeId',
    #'GuidNodeId',
    #'ByteStringNodeId',
    #'ExpandedNodeId',
    #'QualifiedName',
    #'LocalizedText',
    #'DataValue',
    #'Variant',
    #'DiagnosticInfo',
    # 
    #
    #structs we should enable or that we haven't checked yet
    #
    #'ExtensionObject',
    'XmlElement',
    #'Node',
    #'InstanceNode',
    #'TypeNode',
    #'ObjectNode',
    #'ObjectTypeNode',
    #'VariableNode',
    #'VariableTypeNode',
    #'ReferenceTypeNode',
    #'MethodNode',
    #'ViewNode',
    #'DataTypeNode',
    #'ReferenceNode',
    #'Argument',
    #'EnumValueType',
    #'TimeZoneDataType',
    #'ApplicationDescription',
    #'RequestHeader',
    #'ResponseHeader',
    #'ServiceFault',
    #'FindServersRequest',
    #'FindServersResponse',
    #'UserTokenPolicy',
    #'EndpointDescription',
    #'GetEndpointsRequest',
    #'GetEndpointsResponse',
    #'RegisteredServer',
    #'RegisterServerRequest',
    #'RegisterServerResponse',
    #'ChannelSecurityToken',
    #'OpenSecureChannelRequest',
    #'OpenSecureChannelResponse',
    #'CloseSecureChannelRequest',
    #'CloseSecureChannelResponse',
    #'SignedSoftwareCertificate',
    #'SignatureData',
    #'CreateSessionRequest',
    #'CreateSessionResponse',
    #'UserIdentityToken',
    #'AnonymousIdentityToken',
    #'UserNameIdentityToken',
    #'X509IdentityToken',
    #'IssuedIdentityToken',
    #'ActivateSessionRequest',
    #'ActivateSessionResponse',
    #'CloseSessionRequest',
    #'CloseSessionResponse',
    #'CancelRequest',
    #'CancelResponse',
    #'NodeAttributes',
    #'ObjectAttributes',
    #'VariableAttributes',
    #'MethodAttributes',
    #'ObjectTypeAttributes',
    #'VariableTypeAttributes',
    #'ReferenceTypeAttributes',
    #'DataTypeAttributes',
    #'ViewAttributes',
    #'AddNodesItem',
    #'AddNodesResult',
    #'AddNodesRequest',
    #'AddNodesResponse',
    #'AddReferencesItem',
    #'AddReferencesRequest',
    #'AddReferencesResponse',
    #'DeleteNodesItem',
    #'DeleteNodesRequest',
    #'DeleteNodesResponse',
    #'DeleteReferencesItem',
    #'DeleteReferencesRequest',
    #'DeleteReferencesResponse',
    #'ViewDescription',
    #'BrowseDescription',
    #'ReferenceDescription',
    #'BrowseResult',
    #'BrowseRequest',
    #'BrowseResponse',
    #'BrowseNextRequest',
    #'BrowseNextResponse',
    #'RelativePathElement',
    #'RelativePath',
    #'BrowsePath',
    #'BrowsePathTarget',
    #'BrowsePathResult',
    #'TranslateBrowsePathsToNodeIdsRequest',
    #'TranslateBrowsePathsToNodeIdsResponse',
    #'RegisterNodesRequest',
    #'RegisterNodesResponse',
    #'UnregisterNodesRequest',
    #'UnregisterNodesResponse',
    #'EndpointConfiguration',
    #'SupportedProfile',
    #'SoftwareCertificate',
    #'QueryDataDescription',
    #'NodeTypeDescription',
    #'QueryDataSet',
    #'NodeReference',
    #'ContentFilterElement',
    #'ContentFilter',
    #'FilterOperand',
    #'ElementOperand',
    #'LiteralOperand',
    #'AttributeOperand',
    #'SimpleAttributeOperand',
    #'ContentFilterElementResult',
    #'ContentFilterResult',
    #'ParsingResult',
    #'QueryFirstRequest',
    #'QueryFirstResponse',
    #'QueryNextRequest',
    #'QueryNextResponse',
    'ReadValueId',
    'ReadRequest',
    'ReadParameters',
    'ReadResult',
    'ReadResponse',
    #'HistoryReadValueId',
    #'HistoryReadResult',
    #'HistoryReadDetails',
    #'ReadEventDetails',
    #'ReadRawModifiedDetails',
    #'ReadProcessedDetails',
    #'ReadAtTimeDetails',
    #'HistoryData',
    #'ModificationInfo',
    #'HistoryModifiedData',
    #'HistoryEvent',
    #'HistoryReadRequest',
    #'HistoryReadResponse',
    'WriteValue',
    'WriteParameters',
    'WriteRequest',
    'WriteResponse',
    'WriteResult',
    #'HistoryUpdateDetails',
    #'UpdateDataDetails',
    #'UpdateStructureDataDetails',
    #'UpdateEventDetails',
    #'DeleteRawModifiedDetails',
    #'DeleteAtTimeDetails',
    #'DeleteEventDetails',
    #'HistoryUpdateResult',
    #'HistoryUpdateEventResult',
    #'HistoryUpdateRequest',
    #'HistoryUpdateResponse',
    #'CallMethodRequest',
    #'CallMethodResult',
    #'CallRequest',
    #'CallResponse',
    #'MonitoringFilter',
    #'DataChangeFilter',
    #'EventFilter',
    #'AggregateConfiguration',
    #'AggregateFilter',
    #'MonitoringFilterResult',
    #'EventFilterResult',
    #'AggregateFilterResult',
    #'MonitoringParameters',
    #'MonitoredItemCreateRequest',
    #'MonitoredItemCreateResult',
    #'CreateMonitoredItemsRequest',
    #'CreateMonitoredItemsResponse',
    #'MonitoredItemModifyRequest',
    #'MonitoredItemModifyResult',
    #'ModifyMonitoredItemsRequest',
    #'ModifyMonitoredItemsResponse',
    #'SetMonitoringModeRequest',
    #'SetMonitoringModeResponse',
    #'SetTriggeringRequest',
    #'SetTriggeringResponse',
    #'DeleteMonitoredItemsRequest',
    #'DeleteMonitoredItemsResponse',
    #'CreateSubscriptionRequest',
    #'CreateSubscriptionResponse',
    #'ModifySubscriptionRequest',
    #'ModifySubscriptionResponse',
    #'SetPublishingModeRequest',
    #'SetPublishingModeResponse',
    #'NotificationMessage',
    #'NotificationData',
    #'DataChangeNotification',
    #'MonitoredItemNotification',
    #'EventNotificationList',
    #'EventFieldList',
    #'HistoryEventFieldList',
    #'StatusChangeNotification',
    #'SubscriptionAcknowledgement',
    #'PublishRequest',
    #'PublishResponse',
    #'RepublishRequest',
    #'RepublishResponse',
    #'TransferResult',
    #'TransferSubscriptionsRequest',
    #'TransferSubscriptionsResponse',
    #'DeleteSubscriptionsRequest',
    #'DeleteSubscriptionsResponse',
    #'ScalarTestType',
    #'ArrayTestType',
    #'CompositeTestType',
    #'TestStackRequest',
    #'TestStackResponse',
    #'TestStackExRequest',
    #'TestStackExResponse',
    #'BuildInfo',
    #'RedundantServerDataType',
    #'EndpointUrlListDataType',
    #'NetworkGroupDataType',
    #'SamplingIntervalDiagnosticsDataType',
    #'ServerDiagnosticsSummaryDataType',
    #'ServerStatusDataType',
    #'SessionDiagnosticsDataType',
    #'SessionSecurityDiagnosticsDataType',
    #'ServiceCounterDataType',
    #'StatusResult',
    #'SubscriptionDiagnosticsDataType',
    #'ModelChangeStructureDataType',
    #'SemanticChangeStructureDataType',
    #'Range',
    #'EUInformation',
    #'ComplexNumberType',
    #'DoubleComplexNumberType',
    #'AxisInformation',
    #'XVType',
    #'ProgramDiagnosticDataType',
    'Annotation']


def reorder_structs(model):
    types = IgnoredEnums + ["Bit", "Char", "CharArray", "Guid", "SByte", "Int8", "Int16", "Int32", "Int64", "UInt8", "UInt16", "UInt32", "UInt64", "DateTime", "Boolean", "Double", "Float", "ByteString", "Byte", "StatusCode", "DiagnosticInfo", "String", "AttributeId"] + [enum.name for enum in model.enums] + ["VariableAccessLevel"]
    waiting = {}
    newstructs = []
    for s in model.structs:
        types.append(s.name)
        s.waitingfor = []
        ok = True
        for f in s.fields:
            if f.uatype not in types:
                if f.uatype in waiting.keys():
                    waiting[f.uatype].append(s)
                    s.waitingfor.append(f.uatype)
                else:
                    waiting[f.uatype] = [s]
                    s.waitingfor.append(f.uatype)
                ok = False
        if ok:
            newstructs.append(s)
            waitings = waiting.pop(s.name, None)
            if waitings:
                for s2 in waitings:
                    s2.waitingfor.remove(s.name)
                    if not s2.waitingfor:
                        newstructs.append(s2)
    if len(model.structs) != len(newstructs):
        print("Error while reordering structs, some structs could not be reinserted, had {} structs, we now have {} structs".format(len(model.structs), len(newstructs)))
        s1 = set(model.structs)
        s2 = set(newstructs)
        rest = s1 -s2
        print("Variant" in types)
        for s in s1-s2:
            print("{} is waiting for: {}".format(s, s.waitingfor))
        #print(s1 -s2)
        #print(waiting)
    model.structs = newstructs

def override_types(model):
    for struct in model.structs:
        for field in struct.fields:
            if field.name in OverrideTypes.keys():
                field.uatype = OverrideTypes[field.name]


class CodeGenerator(object):
    def __init__(self, model, h_path, enum_path, size_path, ser_path, deser_path, const_path):
        self.model = model 
        self.h_path = h_path
        self.enum_path = enum_path
        self.rawsize_path = size_path
        self.serialize_path = ser_path
        self.deserialize_path = deser_path
        self.constructors_path = const_path
        self.h_file = None
        self.cpp_file = None
        self.structs = []

    def run(self):
        #sys.stderr.write("Generating header file {} and C++ files for XML file {}".format(self.h_path, self.input_path) + "\n")
        print("Generating: ", self.h_path)
        self.h_file = open(self.h_path, "w")
        print("Generating: ", self.enum_path)
        self.enum_file = open(self.enum_path, "w")
        print("Generating: ", self.rawsize_path)
        self.rawsize_file = open(self.rawsize_path, "w")
        print("Generating: ", self.serialize_path)
        self.serialize_file = open(self.serialize_path, "w")
        print("Generating: ", self.deserialize_path)
        self.deserialize_file = open(self.deserialize_path, "w")
        print("Generating: ", self.constructors_path)
        self.constructors_file = open(self.constructors_path, "w")

        self.make_header_h()
        self.make_header_enum()
        self.make_header_rawsize()
        self.make_header_serialize()
        self.make_header_deserialize()
        self.make_header_constructors()
        
        for enum in self.model.enums:
            if not enum.name in IgnoredEnums:
                self.make_enum_h(enum)
                self.make_struct_ser(enum)
        for struct in self.model.structs:
            self.rename_fields(struct)
            if struct.name in NeedConstructor:
                struct.needconstructor = True
            #if not struct.name.endswith("Node") and not struct.name.endswith("NodeId"):
            if not struct.name in EnabledStructs:
                self.write_h("\n/* DISABLED")
                self.write_size("\n/* DISABLED")
                self.write_ser("\n/*  DISABLED")
                self.write_deser("\n/*  DISABLED")
                if struct.needconstructor:
                    self.write_const("\n/*  DISABLED")
            self.make_struct_h(struct)
            self.make_struct_ser(struct)
            if struct.isrequest:
                self.make_request_constructors(struct)
            if not struct.name in EnabledStructs:
                self.write_h("*/")
                self.write_size("*/")
                self.write_ser("*/")
                self.write_deser("*/")
                if struct.needconstructor:
                    self.write_const("*/")

        self.make_footer_h()
        self.make_footer_enum()
        self.make_footer_rawsize()
        self.make_footer_serialize()
        self.make_footer_deserialize()
        self.make_footer_constructors()

    def rename_fields(self, struct):
        ##gcc does not want member with same name as a type
        for field in struct.fields:
            if field.name in OverrideNames:
                field.name = OverrideNames[field.name]
            #else:
                #print("Error name same as type: ", field.name, field.get_ctype(), field.uatype)

    def make_struct_h(self, struct):
        self.write_h("")
        if struct.doc: self.write_h("    //", struct.doc)
        name = struct.name
        base = ""
        #if struct.basetype:
            #base = " : public " + struct.basetype
        self.write_h("    struct %s %s\n    {""" % (name, base))
        for field in struct.fields: 
            #if field.sourcetype:
                #continue
            
            if field.get_ctype() == "OpcUa::" + struct.name:
                #we have a problem selv referensing struct
                self.write_h("         std::shared_ptr<{}> {};".format(field.get_ctype(), field.name))
            else:
                self.write_h("        " , field.get_ctype(), field.name + ";")
        if struct.needconstructor:
            self.write_h("\n        ", struct.name + "();")

        self.write_h("    };")

    def make_raw_size(self, struct):
        self.write_size("")
        self.write_size("    template<>")
        self.write_size("    std::size_t RawSize<{}>(const {}& data)".format(struct.name, struct.name))
        self.write_size("    {")

        if type(struct) == gm.Enum:
            self.write_size("        return sizeof({});".format(struct.get_ctype()))
        else:
            self.write_size("        size_t size = 0;")
            for field in struct.fields:
                switch = ""
                if field.switchfield:
                    if field.switchvalue:
                        switch = "if ((data.{}) & (1<<({}))) ".format(field.switchfield, field.switchvalue)
                    else:
                        container = struct.bits[field.switchfield].container
                        idx = struct.bits[field.switchfield].idx
                        switch = "if ((data.{}) & (1<<({}))) ".format(container, idx)
                prefix = ""
                if field.get_ctype() == struct.name:
                    prefix = "*"
                if field.length:
                    self.write_size("        {}size += RawSizeContainer({}data.{});".format(switch, prefix, field.name))
                else:
                    self.write_size("        {}size += RawSize({}data.{});".format(switch, prefix, field.name))
            self.write_size("        return size;")
        self.write_size("    }")
        self.write_size("")

    def make_serialize(self, struct):
        self.write_ser("")
        self.write_ser("    template<>")
        self.write_ser("    void DataSerializer::Serialize<{}>(const {}& data)".format(struct.name, struct.name))
        self.write_ser("    {")
        if type(struct) == gm.Enum:
            self.write_ser("        *this << static_cast<{}>(data);".format(struct.get_ctype()))
        else:
            for idx, field in enumerate(struct.fields):
                if field.name == "Body" and idx != (len(struct.fields) - 1):
                    self.write_ser("        int32_t bodylength = RawSize(data) - RawSize(data.Encoding);")
                    self.write_ser("        if ((data.Encoding) & (1<<(0))) bodylength -= RawSize(data.TypeId);")
                    self.write_ser("        *this << bodylength;")
                    continue

                switch = ""
                if field.switchfield:
                    if field.switchvalue:
                        switch = "if ((data.{}) & (1<<({}))) ".format(field.switchfield, field.switchvalue)
                    else:
                        container = struct.bits[field.switchfield].container
                        idx = struct.bits[field.switchfield].idx
                        switch = "if ((data.{}) & (1<<({}))) ".format(container, idx)
                prefix = ""
                if field.get_ctype() == struct.name:
                    prefix = "*"
                if field.length:
                    self.write_ser("        {}SerializeContainer(*this, {}data.{});".format(switch, prefix, field.name))
                else:
                    self.write_ser("        {}*this << {}data.{};".format(switch, prefix, field.name))
        self.write_ser("    }")
        self.write_ser("")

    def make_deserialize(self, struct):
        self.write_deser("")
        self.write_deser("    template<>")
        self.write_deser("    void DataDeserializer::Deserialize<{}>({}& data)".format(struct.name, struct.name))
        self.write_deser("    {")
        if type(struct) == gm.Enum:
            self.write_deser("        {} tmp;".format(struct.get_ctype()))
            self.write_deser("        *this >> tmp;")
            #self.write_deser("        data = static_cast<{}>(tmp);".format(struct.get_ctype()))
            self.write_deser("        data = static_cast<{}>(tmp);".format(struct.name))
        else:
            for idx, field in enumerate(struct.fields):
                if field.name == "Body" and idx != (len(struct.fields) - 1):
                    self.write_deser("        int32_t tmp; //Not used")
                    self.write_deser("        *this >> tmp;")
                    continue
                switch = ""
                if field.switchfield:
                    if field.switchvalue:
                        switch = "if ((data.{}) & (1>>({}))) ".format(field.switchfield, field.switchvalue)
                    else:
                        container = struct.bits[field.switchfield].container
                        idx = struct.bits[field.switchfield].idx
                        switch = "if ((data.{}) & (1>>({}))) ".format(container, idx)
                if field.length:
                    self.write_deser("        {}DeserializeContainer(*this, data.{});".format(switch, field.name))
                else:
                    self.write_deser("        {}*this >> data.{};".format(switch, field.name))
        self.write_deser("    }")
        self.write_deser("")
    
    def make_request_constructors(self, struct):
        if not struct.needconstructor:
            return
        self.write_const("")
        self.write_const("    ", struct.name + "::" + struct.name + "()")
        #self.write_const("        : TypeId(ObjectId::" + struct.name +"_Encoding_DefaultBinary)")
        self.write_const("        : TypeId(FourByteNodeId((uint16_t)ObjectId::" + struct.name +"_Encoding_DefaultBinary))")
        self.write_const("    {")
        self.write_const("    }")

    def make_struct_ser(self, struct):
        self.make_raw_size(struct)
        self.make_serialize(struct)
        self.make_deserialize(struct)

    def make_enum_h(self, enum):
        self.write_enum("\n")
        if enum.doc: self.write_enum("    //", enum.doc)
        self.write_enum("    enum class %s : %s\n    {" % (enum.name, self.to_enum_type(enum.uatype)))
        for val in enum.values:
            self.write_enum("        ", val.name, "=", val.value + ",")
        self.write_enum("    };")
        #if enum.name.endswith("Mask"):
        self.write_enum("""    inline {name} operator|({name} a, {name} b) {{ return static_cast<{name}>(static_cast<{type}>(a) | static_cast<{type}>(b)); }}""".format(name=enum.name, type=self.to_enum_type(enum.uatype)))
        self.write_enum("""    inline {name} operator&({name} a, {name} b) {{ return static_cast<{name}>(static_cast<{type}>(a) & static_cast<{type}>(b)); }}""".format(name=enum.name, type=self.to_enum_type(enum.uatype)))


    def to_enum_type(self, val):
        #if val == "6":
            #val = "8"
        return "{}_t".format(val.lower())

    def write_h(self, *args):
        self.h_file.write(" ".join(args) + "\n")

    def write_enum(self, *args):
        self.enum_file.write(" ".join(args) + "\n")

    def write_ser(self, *args):
        self.serialize_file.write(" ".join(args) + "\n")

    def write_size(self, *args):
        self.rawsize_file.write(" ".join(args) + "\n")

    def write_deser(self, *args):
        self.deserialize_file.write(" ".join(args) + "\n")

    def write_const(self, *args):
        self.constructors_file.write(" ".join(args) + "\n")

    def make_header_h(self, ):
        self.write_h('''// DO NOT EDIT THIS FILE!
// It is automatically generated from opcfoundation.org schemas.
//

/// @brief Opc Ua Binary.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/protocol/enums.h>
#include <opc/ua/protocol/variable_access_level.h>
#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/nodeid.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/data_value.h>

namespace OpcUa
{''')

    def make_footer_h(self):
        self.write_h('''
} // namespace
    ''')

    def make_header_enum(self, ):
        self.write_enum('''// DO NOT EDIT THIS FILE!
// It is automatically generated from opcfoundation.org schemas.
//

/// @brief Opc Ua Binary.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include <opc/ua/protocol/status_codes.h>

namespace OpcUa
{
  struct DiagnosticInfo;
''')

    def make_footer_enum(self):
        self.write_enum('''
} // namespace
    ''')

    def make_header_rawsize(self, ):
        self.write_size('''// DO NOT EDIT THIS FILE!
// It is automatically generated from opcfoundation.org schemas.
//

/// @author Olivier Roulet-Dubonnet 
/// @email olivier@sintef.no 
/// @brief Opc Ua Binary. 
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"
#include <opc/ua/protocol/protocol.h>

#include <opc/ua/protocol/binary/stream.h>

namespace OpcUa
{   
    namespace Binary
    {''')

    def make_footer_rawsize(self):
        self.write_size('''
   }

} // namespace
    ''')



    def make_header_serialize(self, ):
        self.write_ser('''// DO NOT EDIT THIS FILE!
// It is automatically generated from opcfoundation.org schemas.
//

/// @author Olivier Roulet-Dubonnet 
/// @email olivier@sintef.no 
/// @brief Opc Ua Binary. 
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"
#include <opc/ua/protocol/protocol.h>

#include <opc/ua/protocol/binary/stream.h>

namespace OpcUa
{   
    namespace Binary
    {''')

    def make_footer_serialize(self):
        self.write_ser('''
   }

} // namespace
    ''')

    def make_header_deserialize(self, ):
        self.write_deser('''// DO NOT EDIT THIS FILE!
// It is automatically generated from opcfoundation.org schemas.
//

/// @author Olivier Roulet-Dubonnet 
/// @email olivier@sintef.no 
/// @brief Opc Ua Binary. 
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"
#include <opc/ua/protocol/protocol.h>

#include <opc/ua/protocol/binary/stream.h>

namespace OpcUa
{   
    namespace Binary
    {''')

    def make_footer_deserialize(self):
        self.write_deser('''
   }

} // namespace
    ''')


    def make_header_constructors(self, ):
        self.write_const('''
/// @author Olivier Roulet-Dubonnet 
/// @email olivier@sintef.no 
/// @brief Opc Ua Binary. 
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/protocol_auto.h>
#include <opc/ua/protocol/object_ids.h>

namespace OpcUa
{   ''')

    def make_footer_constructors(self):
        self.write_const('''
} // namespace
    ''')




if __name__ == "__main__":
    xmlpath = "Opc.Ua.Types.bsd"
    hpath = "../include/opc/ua/protocol/protocol_auto.h"
    enumpath = "../include/opc/ua/protocol/enums.h"
    serializerpath = "../src/protocol/serialize_auto.cpp"
    rawsizepath = "../src/protocol/rawsize_auto.cpp"
    deserializerpath = "../src/protocol/deserialize_auto.cpp"
    constructorspath = "../src/protocol/constructors_auto.cpp"

    p = gm.Parser(xmlpath)
    model = p.parse()
    gm.add_basetype_members(model)
    gm.add_encoding_field(model)
    gm.remove_duplicates(model)
    gm.remove_vector_length(model)
    gm.split_requests(model)

    #p = Parser(xmlpath)
    #model = p.parse()
    #Changes specific to our C++ implementation
    #reorder_extobjects(model)
    #add_basetype_members(model)
    #add_encoding_field(model)
    #remove_vector_length(model)
    #remove_body_length(model)
    #remove_duplicates(model)
    override_types(model)
    #split_requests(model)
    reorder_structs(model)

    f = open("struct_list.txt", "w")
    f.write("enabled_structs = [\\")
    for name in model.struct_list:
        f.write("\n    #'" + name + "',")
    f.write("]")


    c = CodeGenerator(model, hpath, enumpath, rawsizepath, serializerpath, deserializerpath, constructorspath)
    c.run()


