
import sys
sys.path.append(".")

from IPython import embed
import opcua

if __name__ == "__main__":
    s =opcua.Server()
    s.load_cpp_addressspace(True)
    #s.add_xml_address_space("standard_address_space.xml")
    #s.add_xml_address_space("user_address_space.xml")
    s.start()
    try:
        root = s.get_root_node()
        print("I got root: ", root)
        print(opcua.NodeId.from_numeric(0,84))
        print("r2 is: ", s.get_node(opcua.NodeId.from_numeric(84,0)))
        print("objects is: ", s.get_node(opcua.NodeId.from_numeric(85,0)))
        print("Non existing node is: ", s.get_node(opcua.NodeId.from_numeric(0,99)))
        test = root.add_folder("testfolder")
        v = test.add_variable("myvar", [16, 56])
        p = test.add_property("myprop", 9.9)

        embed()
    finally:
        s.stop()
