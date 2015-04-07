
import sys
import time
sys.path.append(".")

from IPython import embed
import opcua

class SubClient(opcua.SubscriptionHandler):
    def __init_(self, *args):
        self.val = MessageSecurityMode::None
        #self.c = Condition()
    def data_change(self, handle, node, val, attr):
        print("Python: New data change event", handle, node, val, attr)
        self.val = val


if __name__ == "__main__":
    client = opcua.Client(True)
    client.connect("opc.tcp://utgaard:4841")
    #s.connect("opc.tcp://192.168.56.101:48030")
    try:
        root = client.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.get_children())

        #getting node by id since nodejs seems not to support  browsepath
        myvar = client.get_node(opcua.NodeId("ns=1;i=1001"))
        #myvar = root.get_child(["0:MyDevice"])
        #print("yvar is: ", myvar,get_children())
        #myvar = root.get_child(["0:MyDevice", "0:MyVariable1"])
        print("myvar is: ", myvar)
        
        sclt = SubClient()
        sub = client.create_subscription(100, sclt)
        handle = sub.subscribe_data_change(myvar)
        print("Subscribe handle is: ", handle)


        embed()
    finally:
        client.disconnect()
