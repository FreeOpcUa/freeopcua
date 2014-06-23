/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#include <iostream>
#include <opc/ua/node.h>

#include <stdexcept>

#include <opc/ua/client/client.h>

int main(int argc, char** argv)
{
  try
  {
      //std::string endpoint = "opc.tcp://192.168.56.101:48030";
      std::string endpoint = "opc.tcp://127.0.0.1:4841";

      std::cout << "Connecting to: " << endpoint << std::endl;
      OpcUa::RemoteClient clt(endpoint);
      clt.Connect();

      OpcUa::Node root = clt.GetRoot();
      std::cout << "Root node is: " << root << std::endl;
      std::vector<std::string> path({"Objects", "Server"});
      OpcUa::Node server = root.GetChild(path);
      std::cout << "Server node obainted by path: " << server << std::endl;

      std::cout << "Child of objects node are: " << std::endl;

      for (OpcUa::Node node : clt.GetObjectsFolder().GetChildren())
        std::cout << "    " << node << std::endl;

      std::cout << "NamespaceArray is: " << std::endl;
      std::vector<std::string> nspath ({"Objects", "Server", "NamespaceArray"});
      OpcUa::Node nsnode = root.GetChild(nspath);
      OpcUa::Variant ns  = nsnode.GetValue();

      for (std::string d : ns.Value.String)
        std::cout << "    "  << d << std::endl;

      /*
      std::vector<std::string> nspath ({"Objects", "Server", "NamespaceArray"});
        OpcUa::Node node = serv.GetChildNode("ServiceLevel");
        if (nsnode) {
          std::cout << "ServiceLevel is: " << node << std::endl;
            OpcUa::Variant ns  = node.ReadValue();
            for (uint d : ns.Value.Byte) {

                std::cout << "ServiceLevel is: "  << d << std::endl;
            }
        }
      }
      */
      std::cout << "Disconnecting" << std::endl;
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

