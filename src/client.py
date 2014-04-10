
import sys
import time
sys.path.append(".")

from IPython import embed
import opcua
print(sys)

if __name__ == "__main__":
    s = opcua.Client()
    s.set_endpoint("opc.tcp://localhost:4841")
    #s.set_endpoint("opc.tcp://192.168.56.101:48030")
    s.connect()
    try:
        root = s.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.browse())
        print("Objects is: ", s.get_objects_node())
        print("Non Existing node is: ", s.get_node(opcua.NodeId.from_numeric(0, 99)))
        vec = opcua.StringVector()
        qs = root.get_child(["0:Objects", "3:Makino02", "3:InputQueueSize"])
        print("InputQueueSize node is: ", qs)
        print("InputQueueSize value is: ", qs.read_value())
        o = s.get_objects_node()
        print("Children of objects are: ", o.browse())
        n = root.get_child(["0:Objects", "0:MakinoInBuffer", "0:Size"])
        print("My node: ", n)
        print("Val is: ", n.read_value())
        n.write_value([9.5, 3.1])
        print("Val is: ", n.read_value())
        #n = root.get_child(["0:Objects"])


        embed()
    finally:
        s.disconnect()
