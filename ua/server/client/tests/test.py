#!/usr/bin/python
import unittest
import test_opcua as opcua

endpointUrl = "opc.tcp://cool_server:4841"
computer = opcua.Computer(endpointUrl)

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
        self.assertEqual(token.policy_id, "PolicyID", "UserTokenPolicy has unexpected PolicyID: " + token.policy_id)
        self.assertEqual(token.issued_token_type, "IssuedTokenType", "UserTokenPolicy has unexpected IssuedTokenType: " + token.issued_token_type)
        self.assertEqual(token.issuer_endpoint_url, "IssuerEndpointURL", "UserTokenPolicy has unexpected IssuedEndpointURL: " + token.issuer_endpoint_url)
        self.assertEqual(token.security_policy_uri, "SecurityPolicyURI", "UserTokenPolicy has unexpected SecurityPolicyURI: " + token.security_policy_uri)
        self.assertEqual(token.token_type, opcua.UserIdentifyTokenType.USERNAME, "UserTokenPolicy has unexpected UserIdentifyTokenType: " + str(token.token_type))

        self._check_application(endpoint.server_description)

    def _check_application(self, app):
        self.assertEqual(app.name, "Name", "Application has invalid name.")
        self.assertEqual(app.uri, "URI", "Application has invalid uri.")
        self.assertEqual(app.product_uri, "ProductURI", "Application has invalid ProductURI.")
        self.assertEqual(app.type, opcua.ApplicationType.CLIENT, "Application has unexpected Type of application.")
        self.assertEqual(app.gateway_server_uri, "GatewayServerURI", "Application has invalid GatewayServerURI.")
        self.assertEqual(app.discovery_profile_uri, "DiscoveryProfileURI", "Application has invalid DiscoveryProfileURI.")
        self.assertEqual(len(app.discovery_urls), 1, "Application has invalid number of DiscoveryURLs.")
        self.assertEqual(app.discovery_urls[0], endpointUrl, "Application has invalid Endpoint url: " + app.discovery_urls[0])
       

if __name__ == '__main__':
    unittest.main()
