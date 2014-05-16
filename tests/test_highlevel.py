
import unittest
from multiprocessing import Process, Event
import time


import opcua

class AllTests(object):
    def test_root(self):
        root = self.opc.get_root_node()
        self.assertEqual(opcua.QualifiedName(0, "Root"), root.get_name())
        nid = opcua.NodeID(0, 84) 
        self.assertEqual( nid ,root.get_id())

    def test_folder(self):
        objects = self.opc.get_objects_node()
        self.assertEqual(opcua.QualifiedName(0, "Objects"), objects.get_name())
        nid = opcua.NodeID(0, 85) 
        self.assertEqual( nid ,objects.get_id())

    def test_addressspace(self):
        objects = self.opc.get_objects_node()
        f = objects.add_folder("3:MyFolder")
        v = f.add_variable("3:MyVariable", 6)
        p = f.add_property("3:MyProperty", 10)
        childs = f.get_children()
        print(v, childs)
        self.assertTrue( v in childs)
        self.assertTrue( p in childs)

    def test_simple_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableTestValue", 4.32)
        val = v.get_value()
        self.assertEqual(4.32, val)

    def test_array_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableArrayValue", [1,2,3])
        val = v.get_value()
        self.assertEqual([1,2,3], val)
        v.set_value([1])
        val = v.get_value()
        self.assertEqual(1, val) #This should be fixed!!! it should be [1]


class ServerProcess(Process):

    def __init__(self):
        Process.__init__(self)
        self._stop = Event()
        self.started = Event()

    def run(self):
        self.srv = opcua.Server()
        self.srv.load_cpp_addressspace(True)
        self.srv.set_endpoint("opc.tcp://localhost:4841")
        self.srv.start()
        self.started.set()
        while not self._stop.is_set():
            time.sleep(0.1)
        print("Stopping server")
        self.srv.stop()
        print("Server stopped")

    def stop(self):
        self._stop.set()


class TestClient(unittest.TestCase, AllTests):
    @classmethod
    def setUpClass(self):
        #start server in its own process
        self.srv = ServerProcess()
        self.srv.start()
        self.srv.started.wait() # let it initialize

        #start client
        self.clt = opcua.Client();
        self.clt.set_endpoint("opc.tcp://localhost:4841")
        self.clt.connect()
        self.opc = self.clt

    @classmethod
    def tearDownClass(self):
        print("Disconnecting")
        self.clt.disconnect()
        print("Trying to stop server")
        self.srv.stop()

class TestServer(unittest.TestCase, AllTests):
    @classmethod
    def setUpClass(self):
        self.srv = opcua.Server()
        self.srv.load_cpp_addressspace(True)
        self.srv.set_endpoint("opc.tcp://localhost:4843")
        self.srv.start()
        self.opc = self.srv 

    @classmethod
    def tearDownClass(self):
        self.srv.stop()





if __name__ == "__main__":
    unittest.main()

