opcua-python
============

OPC-UA Python bindings

Writting an OPC-UA server is as simple as:

    s = opcua.Server()
    s.set_endpoint("opc.tcp://192.168.56.1:4841")
    s.start()
    try:
        objects = s.get_objects_node()

        m = objects.add_folder("2:MyFolder")
        v = m.add_variable("2:MyVariable", 3)
    
        count = 0
        while True;
            count += 1
            v.set_value(count)
            time.sleep(1);

    finally:
        s.stop()
