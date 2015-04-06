
import sys
import time
sys.path.append(".")

from IPython import embed
import opcua

class SubHandler(opcua.SubscriptionHandler):
    def __init__(self, *args):
        opcua.SubscriptionHandler.__init__(self, *args)
        self.val = MessageSecurityMode::None

    def data_change(self, handle, node, val, attr):
        print("Python: New data change event", handle, node, val, attr)
        self.val = val

    def event(self, handle, event):
        print("Python: New event", handle, event)
        self.ev = event


if __name__ == "__main__":
    client = opcua.Client(False)
    client.connect("opc.tcp://localhost:53530/OPCUA/SimulationServer/")
    try:
        root = client.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.get_children())
        print("Objects is: ", client.get_objects_node())
        o = client.get_objects_node()
        print("Children of objects are: ", o.get_children())

        myvar = root.get_child(["0:Objects", "5:Simulation", "5:Random1"])
        print("yvar is: ", myvar)
        myfloat = client.get_node("ns=4;s=Float")
        mydouble = client.get_node("ns=4;s=Double")
        myint64 = client.get_node("ns=4;s=Int64")
        myuint64 = client.get_node("ns=4;s=UInt64")
        myint32 = client.get_node("ns=4;s=Int32")
        myuint32 = client.get_node("ns=4;s=UInt32")
        
        sclt = SubHandler()
        sub = client.create_subscription(100, sclt)
        handle = sub.subscribe_data_change(myvar)
        print("Subscribe handle is: ", handle)


        embed()
    finally:
        client.disconnect()
