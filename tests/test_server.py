
import unittest

import opcua

class AllTests(object):
    def test_root(self):
        root = self.srv.get_root_node()
        self.assertEqual(opcua.QualifiedName(0, "Root"), root.get_name())
        nid = opcua.NodeID(0, 84) 
        self.assertEqual( nid ,root.get_id())

    def test_folder(self):
        objects = self.srv.get_objects_node()
        self.assertEqual(opcua.QualifiedName(0, "Objects"), objects.get_name())
        nid = opcua.NodeID(0, 85) 
        self.assertEqual( nid ,objects.get_id())

    def test_addressspace(self):
        objects = self.srv.get_objects_node()
        f = objects.add_folder("3:MyFolder")
        v = f.add_variable("3:MyVariable", 6)
        p = f.add_property("3:MyProperty", 10)
        childs = f.get_children()
        print(v, childs)
        self.assertTrue( v in childs)
        self.assertTrue( p in childs)

    def test_simple_value(self):
        o = self.srv.get_objects_node()
        v = o.add_variable("3:VariableTestValue", 4.32)
        val = v.get_value()
        self.assertEqual(4.32, val)

    def test_array_value(self):
        o = self.srv.get_objects_node()
        v = o.add_variable("3:VariableArrayValue", [1,2,3])
        val = v.get_value()
        self.assertEqual([1,2,3], val)
        v.set_value([1])
        val = v.get_value()
        self.assertEqual(1, val) #This should be fixed!!! it should be [1]




"""
class TestClient(unittest.TestCase, AllTests):
    @classmethod
    def setUpClass(self):
        self.srv = opcua.Server()
        self.srv.load_cpp_addressspace(True)
        self.srv.set_endpoint("opc.tcp://localhost:4841")
        self.srv.start()
        self.clt = opcua.Client();
        self.clt.set_enpoint("opc.tcp://localhost:4841")
        self.clt.connect()
        self.opc = self.clt

    @classmethod
    def tearDownClass(self):
        self.clt.disconnect()
        self.srv.stop()

"""

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


    #ns = objects.get_child(["0:Server", "0:NamespaceArray"])
    #val.append("http://www.myorganisation.org/Test/")
    #ns.set_value(val)
    #print("Namespace is now: ", ns.get_value())


    #m = objects.add_folder("2:MyFolder")
    #v = m.add_variable("2:MyVariable", 3)
    
    #count = 0
    #while True;
        #count += 1
        #v.set_value(count)
        #time.sleep(1);

