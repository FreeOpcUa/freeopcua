
import unittest

import opcua


class TestServer(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.srv = opcua.Server()
        self.srv.load_cpp_addressspace(True)
        self.srv.set_endpoint("opc.tcp://192.168.56.1:4841")
        self.srv.start()

    def test_root(self):
        root = self.srv.get_root_node()
        self.assertEqual(opcua.QualifiedName(0, "Root"), root.get_name())

    def test_folder(self):
        objects = self.srv.get_objects_node()
        self.assertEqual(opcua.QualifiedName(0, "Objects"), objects.get_name())


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

