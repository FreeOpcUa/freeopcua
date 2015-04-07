
import sys
import time
sys.path.append(".")

from IPython import embed
import opcua

class SubHandler(opcua.SubscriptionHandler):
    """
    Client to subcsription. It will receive events from server
    """
    def __init__(self, *args):
        opcua.SubscriptionHandler.__init__(self, *args)
        self.ev = MessageSecurityMode::None

    def data_change(self, handle, node, val, attr):
        print("Python: New data change event", handle, node, val, attr)

    def event(self, handle, event):
        print("Python: New event", handle, event)
        self.ev = event


if __name__ == "__main__":
    # create our client object
    client = opcua.Client(False)
    client.connect("opc.tcp://localhost:4841/freeopcua/server/")
    #s.connect("opc.tcp://192.168.56.101:48030")
    try:
        # get server namespace. You may want to get all namespaces
        # with client.get_server_namespaces()
        uri = "http://examples.freeopcua.github.io"
        idx = client.get_namespace_index(uri)
        
        # read a node from standard opcua address space
        statenode = client.get_node(opcua.ObjectId.Server_ServerStatus_State)
        print("Server state is: ", statenode.get_value())

        # get root node of server and browse it
        root = client.get_root_node()
        print("I got root: ", root)
        print("Childs are: ", root.get_children())

        # get objects node of server 
        # this is where the interesting data from server should be
        print("Objects is: ", client.get_objects_node())
        objects = client.get_objects_node()
        print("Children of objects are: ", objects.get_children())
        
        # get child using browse path
        myvar = objects.get_child(["{}:NewObject".format(idx), "MyVariable"])
        print("yvar is: ", myvar)
        
        # create a subsription we will use to subscribe to nodes or events
        sclt = SubHandler()
        sub = client.create_subscription(100, sclt)

        # subscribe to a specific node  
        #handle = sub.subscribe_data_change(myvar)
        #print("Subscribe handle is: ", handle)

        # subscribe to events from server  
        evhandle = sub.subscribe_events()
        print("Subscribe handle is: ", evhandle)


        embed()
    finally:
        client.disconnect()
