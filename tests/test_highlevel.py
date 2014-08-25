from IPython import embed
import sys
import unittest
from multiprocessing import Process, Event
import time
from threading import Condition


import opcua


class SubClient(opcua.SubscriptionClient):
    def data_change(self, handle, node, val, attr):
        print("New data change event", node, val, attr)


class Unit(unittest.TestCase):
    def test_zero_nodeid(self):
        nid = opcua.NodeID()
        zero = opcua.NodeID(0, 0)
        self.assertEqual(nid, zero)

    def test_string_nodeid(self):
        nid = opcua.NodeID("titi", 1)
        self.assertEqual(nid.namespace_index, 1)
        self.assertEqual(nid.identifier, "titi")

    def test_numeric_nodeid(self):
        nid = opcua.NodeID(999, 2)
        self.assertEqual(nid.namespace_index, 2)
        self.assertEqual(nid.identifier, 999)

    def test_qualified_name(self):
        qn = opcua.QualifiedName("qname", 2)
        self.assertEqual(qn.namespace_index, 2)
        self.assertEqual(qn.name, "qname")


class CommonTests(object):
    def test_root(self):
        root = self.opc.get_root_node()
        self.assertEqual(opcua.QualifiedName("Root", 0), root.get_name())
        nid = opcua.NodeID(84, 0) 
        self.assertEqual(nid, root.get_id())

    def test_objects(self):
        objects = self.opc.get_objects_node()
        self.assertEqual(opcua.QualifiedName("Objects", 0), objects.get_name())
        nid = opcua.NodeID(85, 0) 
        self.assertEqual(nid, objects.get_id())

    def test_add_nodes(self):
        objects = self.opc.get_objects_node()
        f = objects.add_folder("3:MyFolder")
        v = f.add_variable("3:MyVariable", 6)
        p = f.add_property("3:MyProperty", 10)
        childs = f.get_children()
        self.assertTrue(v in childs)
        self.assertTrue(p in childs)

    def test_add_numeric_variable(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable("ns=3;i=888;", "3:numericnodefromstring", 99)
        nid = opcua.NodeID(888, 3) 
        qn = opcua.QualifiedName("numericnodefromstring", 3) 
        self.assertEqual(nid, v.get_id())
        self.assertEqual(qn, v.get_name())

    def test_add_string_variable(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable("ns=3;s=stringid;", "3:stringnodefromstring", [68])
        nid = opcua.NodeID("stringid", 3) 
        qn = opcua.QualifiedName("stringnodefromstring", 3) 
        self.assertEqual(nid, v.get_id())
        self.assertEqual(qn, v.get_name())

    def test_add_string_array_variable(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable("ns=3;s=stringarrayid;", "9:stringarray", ["l", "b"])
        nid = opcua.NodeID("stringarrayid", 3) 
        qn = opcua.QualifiedName("stringarray", 9) 
        self.assertEqual(nid, v.get_id())
        self.assertEqual(qn, v.get_name())
        val = v.get_value()
        self.assertEqual(["l", "b"], val)



    def test_add_numeric_node(self):
        objects = self.opc.get_objects_node()
        nid = opcua.NodeID(9999, 3)
        qn = opcua.QualifiedName("AddNodeVar1", 3)
        v1 = objects.add_variable(nid, qn, 0)
        self.assertEqual(nid, v1.get_id())
        self.assertEqual(qn, v1.get_name())

    def test_add_string_node(self):
        objects = self.opc.get_objects_node()
        qn = opcua.QualifiedName("AddNodeVar2", 3)
        nid = opcua.NodeID("AddNodeVar2Id", 3)
        v2 = objects.add_variable(nid, qn, 0)
        self.assertEqual(nid, v2.get_id())
        self.assertEqual(qn, v2.get_name())

    def test_add_find_node_(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object("ns=2;i=101;", "2:AddFindObject")
        o2 = objects.get_child("2:AddFindObject")
        self.assertEqual(o, o2)

    def test_node_path(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object("ns=2;i=105;", "2:NodePathObject")
        root = self.opc.get_root_node()
        o2 = root.get_child(["0:Objects", "2:NodePathObject"])
        self.assertEqual(o, o2)

    def test_add_read_node(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object("ns=2;i=102;", "2:AddReadObject")
        nid = opcua.NodeID(102, 2)
        self.assertEqual(o.get_id(), nid)
        qn = opcua.QualifiedName("AddReadObject", 2)
        self.assertEqual(o.get_name(), qn)

    def test_simple_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableTestValue", 4.32)
        val = v.get_value()
        self.assertEqual(4.32, val)

    def test_add_exception(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object("ns=2;i=103;", "2:AddReadObject")
        with self.assertRaises(RuntimeError):
            o2 = objects.add_object("ns=2;i=103;", "2:AddReadObject")

    def test_negative_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableNegativeValue", 4)
        v.set_value(-4.54)
        val = v.get_value()
        self.assertEqual(-4.54, val)

    def test_array_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableArrayValue", [1, 2, 3])
        val = v.get_value()
        self.assertEqual([1, 2, 3], val)

    def test_array_size_one_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:VariableArrayValue", [1, 2, 3])
        v.set_value([1])
        val = v.get_value()
        self.assertEqual([1], val) 

    def test_create_delete_subscription(self):
        o = self.opc.get_objects_node()
        v = o.add_variable("3:SubscriptioinVariable", [1, 2, 3])
        sub = self.opc.create_subscription(100, sclt)
        handle = sub.subscribe_data_change(v)
        time.sleep(0.1)
        sub.unsubscribe(handle)
        sub.delete()


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


    def test_subscription_data_change(self):

        class MySubClient(opcua.SubscriptionClient):
            def setup(self, condition):
                self.cond = condition
                self.hack = False # Fixme should not be necessary, should use condition
                self.node = None
                self.handle = None
                self.attribute = None
                self.value = None

            def data_change(self, handle, node, val, attr):
                print("Data change event in python client", handle, node, val , attr)
                self.handle = handle
                self.node = node
                self.value = val
                self.attribute = attr
                #self.cond.notify_all()
                self.hack = True

        cond = Condition()
        msclt = MySubClient()
        msclt.setup(cond)

        o = self.opc.get_objects_node()
        v1 = o.add_variable("3:SubscriptionVariableV1", [1, 2, 3])
        v2 = o.add_variable("3:SubscriptionVariableV2", 1)
        sub = self.opc.create_subscription(100, msclt)
        handle1 = sub.subscribe_data_change(v1)
        print("Got handle ", handle1)
        handle2 = sub.subscribe_data_change(v2)
        print("Got handle ", handle2)

        v1.set_value([5])
        start = time.time()
        ret = True
        while msclt.hack != True:
            time.sleep(0.05)
            if (time.time() - start) > 0.5:
                ret = False
        msclt.hack = False
        #with cond:
        #    ret = cond.wait(0.5)

        self.assertEqual(ret, True) # we went into timeout waiting for subcsription callback
        self.assertEqual(msclt.value, [5])
        self.assertEqual(msclt.handle, handle1)
        self.assertEqual(msclt.node, v1)

        v2.set_value(99)
        ret = True
        while msclt.hack != True:
            time.sleep(0.05)
            if (time.time() - start) > 0.5:
                ret = False
        msclt.hack = False
        #with cond:
        #with cond:
        #    ret = cond.wait(0.5)
        self.assertEqual(ret, True) # we went into timeout waiting for subcsription callback
        self.assertEqual(msclt.value, 99)
        self.assertEqual(msclt.handle, handle2)
        self.assertEqual(msclt.node, v2)



        sub.unsubscribe(handle1)
        #sub.unsubscribe(handle2) #disabled to test one more case
        sub.delete()





if __name__ == "__main__":
    sclt = SubClient()
    unittest.main(verbosity=3)

