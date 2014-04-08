
import sys
sys.path.append(".")

from IPython import embed
import opcua
print(sys)

if __name__ == "__main__":
    s =opcua.Server()
    s.add_xml_addres_space("standard_address_space.xml")
    s.start()
    try:
        root = s.get_root_node()
        print("I got root: ", root)
        embed()
    finally:
        s.stop()
