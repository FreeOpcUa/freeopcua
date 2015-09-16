#include "opc/ua/server_operations.h"

namespace OpcUa {
	ServerOperations::ServerOperations(Services::SharedPtr Server) : Server(Server)
	{
	}

	void ServerOperations::WriteAttributes(std::vector<WriteValue>& updates)
	{
		std::vector<StatusCode> codes = Server->Attributes()->Write(updates);
		CheckStatusCode(codes.front());
	}
}
