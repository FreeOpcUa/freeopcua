
import sys
import time
sys.path.append(".")

from IPython import embed
import opcua

class SubClient(opcua.SubscriptionClient):
    def __init__(self, *args):
        opcua.SubscriptionClient.__init__(self, *args)
        self.val = None

    def data_change(self, handle, node, val, attr):
        print("Python: New data change event", handle, node, val, attr)
        self.val = val



if __name__ == "__main__":
    client = opcua.Client()
    client.set_endpoint("opc.tcp://localhost:4841")
    #s.set_endpoint("opc.tcp://192.168.56.101:48030")
    client.connect()
    try:
        root = client.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.get_children())
        print("Objects is: ", client.get_objects_node())
        o = client.get_objects_node()
        print("Children of objects are: ", o.get_children())
        myvar = root.get_child(["0:Objects", "0:testfolder", "0:myvar"])
        print("yvar is: ", myvar)
        
        sclt = SubClient()
        sub = client.create_subscription(100, sclt)
        handle = sub.subscribe_data_change(myvar)
        print("Subscribe handle is: ", handle)


        embed()
    finally:
        client.disconnect()
