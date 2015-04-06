
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
    client = opcua.Client(True)
    #client.connect("opc.tcp://localhost:4841")
    client.connect("opc.tcp://utgaard:12685/ctt-server")
    #s.connect("opc.tcp://192.168.56.101:48030")
    #edps = client.get_server_endpoints()
    try:
        root = client.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.get_children())
        print("Objects is: ", client.get_objects_node())
        o = client.get_objects_node()
        print("Children of objects are: ", o.get_children())
        myvar = root.get_child(["0:Objects", "2:NewObject", "2:MyVariable"])
        print("yvar is: ", myvar)
        
        sclt = SubHandler()
        sub = client.create_subscription(100, sclt)
        handle = sub.subscribe_data_change(myvar)
        print("Subscribe handle is: ", handle)
        evhandle = sub.subscribe_events()
        print("Subscribe handle is: ", evhandle)


        embed()
    finally:
        client.disconnect()
