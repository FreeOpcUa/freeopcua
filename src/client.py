
import sys
import time
sys.path.append(".")

from IPython import embed
import opcua

if __name__ == "__main__":
    s = opcua.Client()
    s.set_endpoint("opc.tcp://localhost:4841")
    #s.set_endpoint("opc.tcp://192.168.56.101:48030")
    s.connect()
    try:
        root = s.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.get_children())
        print("Objects is: ", s.get_objects_node())
        o = s.get_objects_node()
        print("Children of objects are: ", o.get_children())


        embed()
    finally:
        s.disconnect()
