
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

    def event(self, handle, event):
        print("Python: New event", handle, event)
        self.ev = event


if __name__ == "__main__":
    client = opcua.Client(False)
    client.set_endpoint("opc.tcp://localhost:4841")
    #s.set_endpoint("opc.tcp://192.168.56.101:48030")
    client.connect()
    try:
        uri = "http://examples.freeopcua.github.io"
        idx = client.get_namespace_index(uri)

        statenode = client.get_node(opcua.ObjectID.Server_ServerStatus_State)
        print("Server state is: ", statenode.get_value())

        root = client.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.get_children())

        print("Objects is: ", client.get_objects_node())
        objects = client.get_objects_node()
        print("Children of objects are: ", objects.get_children())

        myvar = objects.get_child(["{}:NewObject".format(idx), "MyVariable"])
        print("yvar is: ", myvar)
        
        sclt = SubClient()
        sub = client.create_subscription(100, sclt)
        handle = sub.subscribe_data_change(myvar)
        print("Subscribe handle is: ", handle)
        evhandle = sub.subscribe_events()
        print("Subscribe handle is: ", evhandle)


        embed()
    finally:
        client.disconnect()
