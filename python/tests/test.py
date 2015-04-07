#!/usr/bin/python
import unittest
import test_opcua as opcua

endpointUrl = "opc.tcp://cool_server:4841"
computer = opcua.Server(endpointUrl)

class EndpointsTests(unittest.TestCase):
    def test_find_servers(self):
        apps = computer.find_servers()
        self.assertGreater(len(apps), 0, "No applications returned.")

        for app in apps:
            self._check_application(app)

    def test_get_endpoints(self):
        endpoints = computer.get_endpoints()
        self.assertEqual(len(endpoints), 1, "No endpoints returned.")
        
        endpoint = endpoints[0]
        self.assertEqual(endpoint.url, endpointUrl, 'Endpoint has unexpected url: ' + endpoint.url)
        self.assertEqual(endpoint.security_mode, opcua.MessageSecurityMode.NONE, 'Endpoint has unexpected securityMode: ' + str(endpoint.security_mode))
        self.assertEqual(endpoint.security_policy_uri, "SecurityPolicyURI", 'Endpoint has unexpected security policy uri: ' + endpoint.security_policy_uri)
        self.assertEqual(endpoint.transport_profile_uri, "TransportProfileURI", 'Endpoint has unexpected transport profile uri: ' + endpoint.transport_profile_uri)
        self.assertEqual(endpoint.security_level, 1, 'Endpoint has unexpected transport profile uri: ' + str(endpoint.security_level))

        self.assertEqual(len(endpoint.user_identify_tokens), 1, 'Endpoint has number of identify tokens: ' + str(len(endpoint.user_identify_tokens)))
        token = endpoint.user_identify_tokens[0]
        self.assertEqual(token.policy_id, "PolicyId", "UserTokenPolicy has unexpected PolicyId: " + token.policy_id)
        self.assertEqual(token.issued_token_type, "IssuedTokenType", "UserTokenPolicy has unexpected IssuedTokenType: " + token.issued_token_type)
        self.assertEqual(token.issuer_endpoint_url, "IssuerEndpointURL", "UserTokenPolicy has unexpected IssuedEndpointURL: " + token.issuer_endpoint_url)
        self.assertEqual(token.security_policy_uri, "SecurityPolicyURI", "UserTokenPolicy has unexpected SecurityPolicyURI: " + token.security_policy_uri)
        self.assertEqual(token.token_type, opcua.UserIdentifyTokenType.USERNAME, "UserTokenPolicy has unexpected UserIdentifyTokenType: " + str(token.token_type))

        self._check_application(endpoint.server_description)

    def test_browse(self):
        params = opcua.BrowseParameters();
        params.node_to_browse.namespace = 1;
        params.node_to_browse.identifier = opcua.ObjectId.ROOT_FOLDER
        params.direction = opcua.BrowseDirection.BOTH
        params.reference_type_id.namespace = 2;
        params.reference_type_id.identifier = opcua.ObjectId.ORGANIZES
        params.include_subtypes = True
        params.node_classes = opcua.NodeClass.OBJECT | opcua.NodeClass.VARIABLE
        params.result_mask = 2;
        references = computer.browse(params);
        self.assertEqual(len(references), 1, "Number of browsed references is invalid: " + str(len(references)))
        
        ref = references[0]
        self.assertEqual(ref.browse_name.name, "Name", "Unexpected BrowseName at reference: " + str(ref.browse_name.name))
        self.assertEqual(ref.browse_name.namespace_index, 1, "Unexpected NamespaceIndex in browse name: " + str(ref.browse_name.namespace_index))
        self.assertEqual(ref.display_name, "Text", "Unexpected DisplayName: " + str(ref.display_name))
        self.assertEqual(ref.is_forward, True, "Unexpected IsForward: " + str(ref.is_forward))
        self.assertEqual(ref.reference_type_id.namespace_index, 2, "Unexpected reference_type_id.namespace_index: " + str(ref.reference_type_id.namespace_index))
        self.assertEqual(ref.reference_type_id.identifier, "Identifier", "Unexpected reference_type_id.identifier: " + str(ref.reference_type_id.identifier))
        self.assertEqual(ref.target_node_class, opcua.NodeClass.VARIABLE, "Unexpected reference_type_id.identifier: " + str(ref.target_node_class))
        self.assertEqual(ref.target_node_id.namespace_index, 3, "Unexpected target_node_id.namespace_index: " + str(ref.target_node_id.namespace_index))
        self.assertEqual(ref.target_node_id.identifier, 4, "Unexpected target_node_id.identifier: " + str(ref.target_node_id.identifier))
        self.assertEqual(ref.target_node_type_definition.namespace_index, 5, "Unexpected target_node_type_definitions.namespace_index: " + str(ref.target_node_type_definition.namespace_index))
        self.assertEqual(ref.target_node_type_definition.identifier, 6, "Unexpected target_node_type_definition.identifier: " + str(ref.target_node_type_definition.identifier))

    def test_read(self):
        attributeParams = opcua.ReadValueId()
        attributeParams.node.namespace_index = 1
        attributeParams.node.identifier = 2
        attributeParams.attribute = opcua.AttributeId.Value
        attributeParams.index_range = "1:2"
        attributeParams.data_encoding.namespace_index = 3
        attributeParams.data_encoding.name = "binary"
        
        params = opcua.ReadParameters()
        params.max_age = 1;
        params.timestamps_to_return = opcua.TimestampsToReturn.BOTH
        params.attributes_to_read.append(attributeParams);

        values = computer.read(params)
        self.assertEqual(len(values), 1, "Invalid number of read parameters: " + str(len(values)))
        
        data = values[0]
        self.assertEqual(data.value, "value", "Invalid value was read:" + str(data.value))
        self.assertEqual(data.status, 0x806F0000, "Invalid status:" + str(data.status))
        self.assertEqual(data.server_picoseconds, 1, "Invalid server_picoseconds:" + str(data.server_picoseconds))
        self.assertEqual(data.server_timestamp, 2, "Invalid server_timestamp:" + str(data.server_timestamp))
        self.assertEqual(data.source_picoseconds, 3, "Invalid source_picoseconds:" + str(data.source_picoseconds))
        self.assertEqual(data.source_timestamp, 4, "Invalid source_timestamp:" + str(data.source_timestamp))

    def test_write(self):
        value = opcua.WriteValue()
        value.attribute = opcua.AttributeId.Value
        value.node.namespace_index = 1
        value.node.identifier = "node"
        value.numeric_range = "1:2"
        value.data.value = "value"
        value.data.status = 0x806F0000
        value.data.server_picoseconds = 1
        value.data.server_timestamp = 2
        value.data.source_picoseconds = 3
        value.data.source_timestamp = 4
        
        statuses = computer.write([value]);
        self.assertEqual(len(statuses), 1, "Invalid number of statuses.")
        self.assertEqual(statuses[0], 0x806F0000, "Invalid status.")

    def _check_application(self, app):
        self.assertEqual(app.name, "Name", "Application has invalid name.")
        self.assertEqual(app.uri, "URI", "Application has invalid uri.")
        self.assertEqual(app.product_uri, "ProductURI", "Application has invalid ProductURI.")
        self.assertEqual(app.type, opcua.ApplicationType.Client, "Application has unexpected Type of application.")
        self.assertEqual(app.gateway_server_uri, "GatewayServerURI", "Application has invalid GatewayServerURI.")
        self.assertEqual(app.discovery_profile_uri, "DiscoveryProfileURI", "Application has invalid DiscoveryProfileURI.")
        self.assertEqual(len(app.discovery_urls), 1, "Application has invalid number of DiscoveryURLs.")
        self.assertEqual(app.discovery_urls[0], endpointUrl, "Application has invalid Endpoint url: " + app.discovery_urls[0])
       

if __name__ == '__main__':
    unittest.main()
