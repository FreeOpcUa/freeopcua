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
        
        

## Demo GUI for real-time data visualization (gui-demo.py)

>### Requirements:
1) Simulation server (Eg. Prosys simulation server)
2) IDE with python libraries installed
3) MySql database
4) OPCUA Python library

>### Features:
1) This script enables you to visualize the OPC values in real-time with the facility of data logging. 
2) An alert will be generated when the downtime reaches a threshold limit.
3) Values can be adjusted as per the need.

>### Screenshot of GUI:

![3](https://user-images.githubusercontent.com/44259288/94888861-0330c500-0498-11eb-92a6-82e28eb16a17.jpg)
