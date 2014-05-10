opcua-python
============

opcua python bindings

Writting an OPC-UA server is as simple as:
s = opcua.Server()
s.load_cpp_addressspace(True)
s.set_endpoint("opc.tcp://192.168.56.1:4841")
s.start()
try;
    objects = s.get_objects_node()

    ns = objects.get_child(["0:Server", "0:NamespaceArray"])
    val.append("http://www.myorganisation.org/Test/")
    ns.set_value(val)
    print("Namespace is now: ", ns.get_value())


    m = objects.add_folder("2:MyFolder")
    v = m.add_variable("2:MyVariable", 3)
    
    count = 0
    while True;
        count += 1
        v.set_value(count)
        time.sleep(1);

finally:
    s.stop()
