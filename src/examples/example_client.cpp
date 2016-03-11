/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>

#include <iostream>
#include <stdexcept>
#include <thread>

using namespace OpcUa;

int main(int argc, char** argv)
{
	try
	{
		//std::string endpoint = "opc.tcp://192.168.56.101:48030";
		//std::string endpoint = "opc.tcp://user:password@192.168.56.101:48030";
		std::string endpoint = "opc.tcp://admin@141.3.80.67:21553/";
		//std::string endpoint = "opc.tcp://localhost:53530/OPCUA/SimulationServer/";
		//std::string endpoint = "opc.tcp://localhost:48010";

		if (argc > 1)
			endpoint = argv[1];

		std::cout << "Connecting to: " << endpoint << std::endl;
		bool debug = true;
		OpcUa::UaClient client(debug);
		client.Connect(endpoint);

		//get Root node on server
		OpcUa::Node root = client.GetRootNode();
		std::cout << "Root node is: " << root << std::endl;

		//get and browse Objects node
		std::cout << "Child of objects node are: " << std::endl;
		Node objects = client.GetObjectsNode();
		for (OpcUa::Node node : objects.GetChildren())
			std::cout << "    " << node << std::endl;

    std::cout << "Trying to get Interaction 1 " << std::endl;
    OpcUa::Node myvar;
    std::vector<std::string> varpath{"2:CurrentWorkpiece", "2:Interaction", "2:1", "2:Name"};
    myvar = objects.GetChild(varpath);
    std::cout << "got node: " << myvar <<std::endl;
    std::vector<OpcUa::Node> nodes;
    nodes.push_back(myvar);
    client.DeleteNodes(nodes);

		//Initialize Node myvar:

		//Get namespace index we are interested in

		// From freeOpcUa Server:
		//uint32_t idx = client.GetNamespaceIndex("http://examples.freeopcua.github.io");
		////Get Node using path (BrowsePathToNodeId call)
		//std::vector<std::string> varpath({ std::to_string(idx) + ":NewObject", "MyVariable" });
		//myvar = objects.GetChild(varpath);

		// Example data from Prosys server:
		//std::vector<std::string> varpath({"Objects", "5:Simulation", "5:Random1"});
		//myvar = root.GetChild(varpath);

		// Example from any UA server, standard dynamic variable node:
		varpath = { "Objects", "Server", "ServerStatus", "CurrentTime" };
		myvar = root.GetChild(varpath);

		std::cout << "got node: " << myvar << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(5));

		std::cout << "Disconnecting" << std::endl;
		client.Disconnect();
		return 0;
	}
	catch (const std::exception& exc)
	{
		std::cout << exc.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown error." << std::endl;
	}
	return -1;
}

