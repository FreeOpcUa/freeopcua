
import sys
import time
sys.path.append(".")

from IPython import embed
import libopcua as opcua
print(sys)

if __name__ == "__main__":
    s = opcua.Client()
    s.set_endpoint("opc.tcp://localhost:4841")
    s.set_endpoint("opc.tcp://192.168.56.101:48030")
    s.connect()
    try:
        root = s.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.browse())
        print("Childs are: ", list(root.browse()))
        print("Objects is: ", s.get_objects_node())
        print("Idoiot is: ", s.get_node(opcua.NodeId.from_numeric(0, 99)))
        vec = opcua.StringVector()
        for i in ["0:Objects", "3:Makino02", "3:InputQueueSize"]:
            vec.append(i)
        qs = root.get_child(vec)
        print("InputQueueSize node is: ", qs)
        print("InputQueueSize value is: ", qs.read_value())

        embed()
    finally:
        s.disconnect()
