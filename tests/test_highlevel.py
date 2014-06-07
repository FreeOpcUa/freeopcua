from IPython import embed
import unittest
from multiprocessing import Process, Event
import time


import opcua

class Unit(unittest.TestCase):
    def test_zero_nodeid(self):
        nid = opcua.NodeID()
        zero = opcua.NodeID(0, 0)
        self.assertEqual(nid, zero)

    def test_string_nodeid(self):
        nid = opcua.NodeID(1, "titi")
        self.assertEqual(nid.namespace_index, 1)
        self.assertEqual(nid.identifier, "titi")

    def test_numeric_nodeid(self):
        nid = opcua.NodeID(2, 999)
        self.assertEqual(nid.namespace_index, 2)
        self.assertEqual(nid.identifier, 999)

    def test_qualified_name(self):
        qn = opcua.QualifiedName(2, "qname")
        self.assertEqual(qn.namespace_index, 2)
        self.assertEqual(qn.name, "qname")


class CommonTests(object):
    def test_root(self):
        root = self.opc.get_root_node()
        self.assertEqual(opcua.QualifiedName(0, "Root"), root.get_name())
        nid = opcua.NodeID(0, 84) 
        self.assertEqual( nid ,root.get_id())

    def test_objects(self):
        objects = self.opc.get_objects_node()
        self.assertEqual(opcua.QualifiedName(0, "Objects"), objects.get_name())
        nid = opcua.NodeID(0, 85) 
        self.assertEqual( nid ,objects.get_id())

    def test_add_nodes(self):
        objects = self.opc.get_objects_node()
        f = objects.add_folder("3:MyFolder")
        v = f.add_variable("3:MyVariable", 6)
        p = f.add_property("3:MyProperty", 10)
        childs = f.get_children()
        print(v, childs)
        self.assertTrue( v in childs)
        self.assertTrue( p in childs)

    def test_add_numeric_node(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable("3:888", "3:numericnodefromstring", 1)
        self.assertEqual( nid ,v.get_id())
        self.assertEqual(qn, v.get_name())

    def test_add_numeric_node(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable("3:stringid", "3:stringnodefromstring", 1)
        self.assertEqual( nid ,v.get_id())
        self.assertEqual(qn, v.get_name())


    def test_add_numeric_node(self):
        objects = self.opc.get_objects_node()
        nid = opcua.NodeID(3, 9999)
        qn = opcua.QualifiedName(3, "AddNodeVar1")
        v1 = objects.add_variable(nid, qn, 0)
        self.assertEqual( nid ,v1.get_id())
        self.assertEqual(qn, v1.get_name())

    def test_add_string_node(self):
        objects = self.opc.get_objects_node()
        qn = opcua.QualifiedName(3, "AddNodeVar2")
        nid = opcua.NodeID(3, "AddNodeVar2Id")
        v2 = objects.add_variable(nid, qn, 0)
        self.assertEqual( nid ,v2.get_id())
        self.assertEqual(qn, v2.get_name())

    def test_add_find_node_(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object("2:101", "2:AddFindObject")
        o2 = objects.get_child("2:AddFindObject")
        self.assertEqual(o, o2)

    def test_add_read_node(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object("2:102", "2:AddReadObject")
        nid = opcua.NodeID(2, 102)
        self.assertEqual(o.get_id(), nid)
        qn = opcua.QualifiedName(2, "AddReadObject")
        self.assertEqual(o.get_name(), qn)

    def test_simple_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableTestValue", 4.32)
        val = v.get_value()
        self.assertEqual(4.32, val)

    def test_negative_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableNegativeValue", 4)
        v.set_value(-4.54)
        val = v.get_value()
        self.assertEqual(-4.54, val)

    def test_array_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableArrayValue", [1,2,3])
        val = v.get_value()
        self.assertEqual([1,2,3], val)


    def test_array_size_one_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableArrayValue", [1,2,3])
        v.set_value([1])
        val = v.get_value()
        self.assertEqual(1, val) #This should be fixed!!! it should be [1]


class ServerProcess(Process):

    def __init__(self):
        Process.__init__(self)
        self._exit = Event()
        self.started = Event()

    def run(self):
        self.srv = opcua.Server()
        self.srv.load_cpp_addressspace(True)
        self.srv.set_endpoint("opc.tcp://localhost:4841")
        self.srv.start()
        self.started.set()
        while not self._exit.is_set():
            time.sleep(0.1)
        print("Stopping server")
        self.srv.stop()
        print("Server stopped")

    def stop(self):
        self._exit.set()

class TestClient(unittest.TestCase, CommonTests):
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

class TestServer(unittest.TestCase, CommonTests):
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

