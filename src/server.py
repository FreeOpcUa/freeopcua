
import sys
sys.path.append(".")

from IPython import embed
import libopcua as opcua
print(sys)

if __name__ == "__main__":
    s =opcua.Server()
    s.add_xml_address_space("standard_address_space.xml")
    #s.add_xml_address_space("user_address_space.xml")
    s.start()
    try:
        root = s.get_root_node()
        print("I got root: ", root)
        print(opcua.NodeId.from_numeric(0,84))
        print("r2 is: ", s.get_node(opcua.NodeId.from_numeric(84,0)))
        print("objects is: ", s.get_node(opcua.NodeId.from_numeric(85,0)))
        print("test is: ", s.get_node(opcua.NodeId.from_numeric(0,99)))
        embed()
    finally:
        s.stop()
