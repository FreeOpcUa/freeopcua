#pragma once

#include "opc/ua/services/services.h"
#include "opc/ua/node.h"

#include <vector>

namespace OpcUa{
	class UaClient;
	class UaServer;

	class ServerOperations {
		friend UaClient;
		friend UaServer;

		Services::SharedPtr Server;
	protected:
		ServerOperations(Services::SharedPtr Server);

	public:
		void WriteAttributes(std::vector<WriteValue>&);
	};
}