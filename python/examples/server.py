
import sys
sys.path.append(".")

from IPython import embed
import opcua

class SubClient(opcua.SubscriptionClient):
    def data_change(self, handle, node, val, attr):
        print("New data change event", handle, node, val, attr)


if __name__ == "__main__":
    server = opcua.Server(False)
    server.set_endpoint("opc.tcp://localhost:4841/freeopcua/server/")
    server.set_server_name("FreeOpcUa Example Server")
    server.start()
    try:
        uri = "http://examples.freeopcua.github.io"
        idx = server.register_namespace(uri)

        objects = server.get_objects_node()
        print("I got objects folder: ", objects)

        #Now adding some object to our addresse space from server side
        myobject = objects.add_object(idx, "NewObject")
        myvar = myobject.add_variable(idx, "MyVariable", [16, 56])
        myprop = myobject.add_property(idx, "myprop", 9.9)
        myfolder = myobject.add_folder(idx, "myfolder")
       
        # uncomment next lines to subscribe to changes on server side
        #sclt = SubClient()
        #sub = server.create_subscription(100, sclt)
        #handle = sub.subscribe_data_change(myvar) #keep handle if you want to delete the particular subscription later

        embed()
    finally:
        server.stop()

