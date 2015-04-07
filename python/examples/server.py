import datetime
import time
import sys
sys.path.append(".")

from IPython import embed
import opcua

class SubHandler(opcua.SubscriptionHandler):
    def data_change(self, handle, node, val, attr):
        print("New data change event", handle, node, val, attr)

    def event(self, handle, event):
        print("Python: New event", handle, event)


if __name__ == "__main__":
    # create our server, the argument is for debugging
    server = opcua.Server(True)
    server.set_endpoint("opc.tcp://localhost:4841/freeopcua/server/")
    server.set_server_name("FreeOpcUa Example Server")

    # start the server
    server.start()

    try:
        # setup our own namespace
        uri = "http://examples.freeopcua.github.io"
        idx = server.register_namespace(uri)

        # get Objects node, this is where we should put our custom stuff 
        objects = server.get_objects_node()
        print("I got objects folder: ", objects)

        # now adding some object to our addresse space 
        myobject = objects.add_object(idx, "NewObject")
        myvar = myobject.add_variable(idx, "MyVariable", [16, 56])
        myprop = myobject.add_property(idx, "myprop", 9.9)
        myfolder = myobject.add_folder(idx, "myfolder")
       
        # uncomment next lines to subscribe to changes on server side
        #sclt = SubHandler()
        #sub = server.create_subscription(100, sclt)
        #handle = sub.subscribe_data_change(myvar) #keep handle if you want to delete the particular subscription later
        
        ev = opcua.Event()
        counter = 0
        while True:
            counter += 1
            ev.message = "This is event nr: " + str(counter)
            ev.source_node = objects.get_id()
            ev.time = datetime.datetime.now()
            print("Sending event: ", ev)
            server.trigger_event(ev)
            myvar.set_value([counter, counter+10])
            time.sleep(1)
        # start ipython shell so users can test things
        #embed()
    finally:
        server.stop()

