
import unittest

import opcua


class TestServer(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.srv = opcua.Server()
        self.srv.load_cpp_addressspace(True)
        self.srv.set_endpoint("opc.tcp://192.168.56.1:4841")
        self.srv.start()

    @classmethod
    def tearDownClass(self):
        self.srv.stop()


    def test_root(self):
        root = self.srv.get_root_node()
        self.assertEqual(opcua.QualifiedName(0, "Root"), root.get_name())

    def test_folder(self):
        objects = self.srv.get_objects_node()
        self.assertEqual(opcua.QualifiedName(0, "Objects"), objects.get_name())

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
        self.assertEqual([1], val)






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

