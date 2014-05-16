
import sys
sys.path.append(".")

from IPython import embed
import opcua

if __name__ == "__main__":
    s = opcua.Server()
    s.set_endpoint("opc.tcp://localhost:4841")
    s.load_cpp_addressspace(True)
    #s.add_xml_address_space("standard_address_space.xml")
    #s.add_xml_address_space("user_address_space.xml")
    s.start()
    try:
        root = s.get_root_node()
        print("I got root: ", root)
        test = root.add_folder("testfolder")
        v = test.add_variable("myvar", [16, 56])
        p = test.add_property("myprop", 9.9)

        embed()
    finally:
        s.stop()
