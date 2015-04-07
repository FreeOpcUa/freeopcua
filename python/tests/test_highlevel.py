#! /usr/bin/env python

import sys
sys.path.insert(0, "../../build/bin/")
import datetime
import unittest
from threading import Thread, Event
try:
    from queue import Queue
except ImportError:
    from Queue import Queue
import time
from threading import Condition
import opcua

port_num1 = 48410
port_num2 = 48430

class SubHandler(opcua.SubscriptionHandler):
    '''
        Dummy subscription client
    '''
    def data_change(self, handle, node, val, attr):
        pass    

    def event(self, handle, event):
        pass 

class MySubHandler(opcua.SubscriptionHandler):
    '''
    More advanced subscription client using conditions, so we can wait for events in tests 
    '''
    def setup(self):
        self.cond = Condition()
        self.node = None
        self.handle = None
        self.attribute = None
        self.value = None
        self.ev = None
        return self.cond

    def data_change(self, handle, node, val, attr):
        self.handle = handle
        self.node = node
        self.value = val
        self.attribute = attr
        with self.cond:
            self.cond.notify_all()

    def event(self, handle, event):
        self.ev = event
        with self.cond:
            self.cond.notify_all()

class Unit(unittest.TestCase):
    '''
    Simple unit test that do not need to setup a server or a client 
    '''

    def test_equal_nodeid(self):
        nid1 = opcua.NodeId(999, 2)
        nid2 = opcua.NodeId(999, 2)
        self.assertTrue(nid1==nid2)
        self.assertTrue(id(nid1)!=id(nid2))
    
    def test_zero_nodeid(self):
        self.assertEqual(opcua.NodeId(), opcua.NodeId(0,0))
        self.assertEqual(opcua.NodeId(), opcua.NodeId('ns=0;i=0;'))

    def test_string_nodeid(self):
        nid = opcua.NodeId('titi', 1)
        self.assertEqual(nid.namespace_index, 1)
        self.assertEqual(nid.identifier, 'titi')
        self.assertTrue(nid.is_string)

    def test_numeric_nodeid(self):
        nid = opcua.NodeId(999, 2)
        self.assertEqual(nid.namespace_index, 2)
        self.assertEqual(nid.identifier, 999)
        self.assertTrue(nid.is_integer)

    def test_qualifiedstring_nodeid(self):
        nid = opcua.NodeId('ns=2;s=PLC1.Manufacturer;')
        self.assertEqual(nid.namespace_index, 2)
        self.assertEqual(nid.identifier, 'PLC1.Manufacturer')

    
    def test_strrepr_nodeid(self):
        nid = opcua.NodeId('ns=2;s=PLC1.Manufacturer;')
        self.assertEqual(str(nid), 'ns=2;s=PLC1.Manufacturer;')
        self.assertEqual(repr(nid), 'ns=2;s=PLC1.Manufacturer;')
    
    def test_qualified_name(self):
        qn = opcua.QualifiedName('qname', 2)
        self.assertEqual(qn.namespace_index, 2)
        self.assertEqual(qn.name, 'qname')
        self.assertEqual(repr(qn), 'QualifiedName(2:qname)')
        self.assertEqual(repr(qn), str(qn))
        qn2 = opcua.QualifiedName(2,'qname')
        self.assertEqual(qn,qn2)
    
    def test_datavalue(self):
        dv = opcua.DataValue(123)
        self.assertEqual(dv.value, 123)
        dv = opcua.DataValue('abc')
        self.assertEqual(dv.value, 'abc')
        tnow = int(time.time())
        dv.source_timestamp = opcua.DateTime.from_time_t(tnow)
        self.assertEqual(dv.source_timestamp.to_time_t(), tnow)
        dv = opcua.DataValue(True, opcua.VariantType.BOOLEAN)
        self.assertEqual(dv.value, True)
        self.assertEqual(type(dv.value), bool)

    def test_application_description(self):
        ad=opcua.ApplicationDescription()
        self.assertEqual(ad.type,opcua.ApplicationType.Client)
        ad.discovery_urls=['a','b','c']
        self.assertEqual(ad.discovery_urls,['a','b','c'])
    
    def test_user_token_policy(self):
        utp = opcua.UserTokenPolicy()
        self.assertEqual(utp.token_type,opcua.UserIdentifyTokenType.ANONYMOUS)

    def test_endpoint_description(self):
        ed=opcua.EndpointDescription()
        self.assertEqual(ed.security_mode, opcua.MessageSecurityMode.None_)
        self.assertEqual(ed.security_level,0)
        ed.server_description=opcua.ApplicationDescription()
        self.assertEqual(ed.user_identify_tokens,[])
        ed.user_identify_tokens = [opcua.UserTokenPolicy()]*3
        self.assertEqual(len(ed.user_identify_tokens),3)

    def test_reference_description(self):
        rd=opcua.ReferenceDescription()
        self.assertEqual(rd.browse_name,opcua.QualifiedName())
        self.assertEqual(rd.is_forward,False)
        self.assertEqual(rd.reference_type_id,opcua.NodeId())
        self.assertEqual(rd.target_node_class,opcua.NodeClass.Unspecified)
        self.assertEqual(rd.target_node_id,opcua.NodeId())
        self.assertEqual(rd.target_node_type_definition,opcua.NodeId())

    def test_attribute_valueid(self):
        avid=opcua.ReadValueId()
        self.assertEqual(avid.node_id, opcua.NodeId())
        self.assertEqual(avid.attribute_id, opcua.AttributeId())
        self.assertEqual(avid.index_range, '')
        self.assertEqual(avid.data_encoding, opcua.QualifiedName())

    def test_write_value(self):
        wv=opcua.WriteValue()
        self.assertEqual(wv.node_id, opcua.NodeId())
        self.assertEqual(wv.attribute_id, opcua.AttributeId())
        self.assertEqual(wv.index_range,'')
        self.assertEqual(wv.value.value, None)

    def test_datetime(self):
        tnow1 = int(time.time())
        tnow = int((datetime.datetime.utcnow() - datetime.datetime(1970,1,1)).total_seconds())
        self.assertEqual(tnow, tnow1) #if this one fails this is a system error, not freopcua

        dt = opcua.DateTime.from_time_t(tnow)
        self.assertEqual(tnow, dt.to_time_t())

        pydt = dt.to_datetime()
        self.assertEqual(tnow, int((pydt - datetime.datetime(1970,1,1)).total_seconds()))

        dt2 = opcua.DateTime(pydt)
        #self.assertEqual(dt2, dt) #FIXME: not implemented
        pydt2 = dt.to_datetime()
        self.assertEqual(pydt, pydt2)

    def test_localized_text(self):
        event = opcua.Event()
        txt = "This is string"
        event.message = txt #message is of type LocalizedText
        self.assertEqual(txt, event.message)



class CommonTests(object):
    '''
    Tests that will be run twice. Once on server side and once on 
    client side since we have been carefull to have the exact 
    same api on server and client side
    '''

    def test_root(self):
        root = self.opc.get_root_node()
        self.assertEqual(opcua.QualifiedName('Root', 0), root.get_browse_name())
        nid = opcua.NodeId(84, 0) 
        self.assertEqual(nid, root.get_id())

    def test_objects(self):
        objects = self.opc.get_objects_node()
        self.assertEqual(opcua.QualifiedName('Objects', 0), objects.get_browse_name())
        nid = opcua.NodeId(85, 0) 
        self.assertEqual(nid, objects.get_id())

    def test_add_nodes(self):
        objects = self.opc.get_objects_node()
        f = objects.add_folder(3, 'MyFolder')
        v = f.add_variable(3, 'MyVariable', 6)
        p = f.add_property(3, 'MyProperty', 10)
        childs = f.get_children()
        self.assertTrue(v in childs)
        self.assertTrue(p in childs)

    def test_add_numeric_variable(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable('ns=3;i=888;', '3:numericnodefromstring', 99)
        nid = opcua.NodeId(888, 3)
        qn = opcua.QualifiedName('numericnodefromstring', 3) 
        self.assertEqual(nid, v.get_id())
        self.assertEqual(qn, v.get_browse_name())

    def test_add_string_variable(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable('ns=3;s=stringid;', '3:stringnodefromstring', [68])
        nid = opcua.NodeId('stringid', 3) 
        qn = opcua.QualifiedName('stringnodefromstring', 3) 
        self.assertEqual(nid, v.get_id())
        self.assertEqual(qn, v.get_browse_name())

    def test_add_string_array_variable(self):
        objects = self.opc.get_objects_node()
        v = objects.add_variable('ns=3;s=stringarrayid;', '9:stringarray', ['l', 'b'])
        nid = opcua.NodeId('stringarrayid', 3) 
        qn = opcua.QualifiedName('stringarray', 9) 
        self.assertEqual(nid, v.get_id())
        self.assertEqual(qn, v.get_browse_name())
        val = v.get_value()
        self.assertEqual(['l', 'b'], val)

    def test_add_numeric_node(self):
        objects = self.opc.get_objects_node()
        nid = opcua.NodeId(9999, 3)
        qn = opcua.QualifiedName('AddNodeVar1', 3)
        v1 = objects.add_variable(nid, qn, 0)
        self.assertEqual(nid, v1.get_id())
        self.assertEqual(qn, v1.get_browse_name())

    def test_add_string_node(self):
        objects = self.opc.get_objects_node()
        qn = opcua.QualifiedName('AddNodeVar2', 3)
        nid = opcua.NodeId('AddNodeVar2Id', 3)
        v2 = objects.add_variable(nid, qn, 0)
        self.assertEqual(nid, v2.get_id())
        self.assertEqual(qn, v2.get_browse_name())

    def test_add_find_node_(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object('ns=2;i=101;', '2:AddFindObject')
        o2 = objects.get_child('2:AddFindObject')
        self.assertEqual(o, o2)

    def test_node_path(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object('ns=2;i=105;', '2:NodePathObject')
        root = self.opc.get_root_node()
        o2 = root.get_child(['0:Objects', '2:NodePathObject'])
        self.assertEqual(o, o2)

    def test_add_read_node(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object('ns=2;i=102;', '2:AddReadObject')
        nid = opcua.NodeId(102, 2)
        self.assertEqual(o.get_id(), nid)
        qn = opcua.QualifiedName('AddReadObject', 2)
        self.assertEqual(o.get_browse_name(), qn)

    def test_simple_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable(3, 'VariableTestValue', 4.32)
        val = v.get_value()
        self.assertEqual(4.32, val)

    def test_add_exception(self):
        objects = self.opc.get_objects_node()
        o = objects.add_object('ns=2;i=103;', '2:AddReadObject')
        with self.assertRaises(RuntimeError):
            o2 = objects.add_object('ns=2;i=103;', '2:AddReadObject')

    def test_negative_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable(3, 'VariableNegativeValue', 4)
        v.set_value(-4.54)
        val = v.get_value()
        self.assertEqual(-4.54, val)

    def test_read_server_state(self):
        statenode = self.opc.get_node(opcua.ObjectId.Server_ServerStatus_State)
        state = statenode.get_value()
        self.assertEqual(state, 0)

    def test_array_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable(3, 'VariableArrayValue', [1, 2, 3])
        val = v.get_value()
        self.assertEqual([1, 2, 3], val)

    def test_array_size_one_value(self):
        o = self.opc.get_objects_node()
        v = o.add_variable(3, 'VariableArrayValue', [1, 2, 3])
        v.set_value([1])
        val = v.get_value()
        self.assertEqual([1], val) 

    def test_create_delete_subscription(self):
        o = self.opc.get_objects_node()
        v = o.add_variable(3, 'SubscriptionVariable', [1, 2, 3])
        sub = self.opc.create_subscription(100, sclt)
        handle = sub.subscribe_data_change(v)
        time.sleep(0.1)
        sub.unsubscribe(handle)
        sub.delete()

    def test_subscribe_events(self):
        sub = self.opc.create_subscription(100, sclt)
        handle = sub.subscribe_events()
        #time.sleep(0.1)
        sub.unsubscribe(handle)
        sub.delete()

    def test_events(self):
        msclt = MySubHandler()
        cond = msclt.setup()
        sub = self.opc.create_subscription(100, msclt)
        handle = sub.subscribe_events()
        
        ev = opcua.Event()
        msg = "this is my msg " 
        ev.message = msg
        tid = datetime.datetime.now()
        ev.time = tid
        ev.source_node = self.opc.get_server_node().get_id()
        ev.source_name = "our server node"
        ev.severity = 500
        self.srv.trigger_event(ev)
        
        with cond:
            ret = cond.wait(50000)
        if sys.version_info.major>2: self.assertEqual(ret, True) # we went into timeout waiting for subcsription callback
        else: pass # XXX
        self.assertIsNot(msclt.ev, None)# we did not receive event
        self.assertEqual(msclt.ev.message, msg)
        self.assertEqual(msclt.ev.time.to_datetime(), tid)
        self.assertEqual(msclt.ev.severity, 500)
        self.assertEqual(msclt.ev.source_node, self.opc.get_server_node().get_id())

        #time.sleep(0.1)
        sub.unsubscribe(handle)
        sub.delete()


    def test_get_namespace_index(self):
        idx = self.opc.get_namespace_index('http://freeopcua.github.io')
        self.assertEqual(idx, 1) 

    def test_use_namespace(self):
        root = self.opc.get_root_node()
        idx = self.opc.get_namespace_index('http://freeopcua.github.io')
        o = root.add_object(idx, 'test_namespace')
        self.assertEqual(idx, o.get_id().namespace_index) 
        o2 = root.get_child('{}:test_namespace'.format(idx))
        self.assertEqual(o, o2) 

    def test_non_existing_node(self):
        root = self.opc.get_root_node()
        with self.assertRaises(RuntimeError):
            server_time_node = root.get_child(['0:Objects', '0:Server', '0:nonexistingnode'])

    def test_subscription_data_change(self):
        '''
        test subscriptions. This is far too complicated for a unittest but, setting up subscriptions requires a lot of code, so when we first set it up, it is best to test as many things as possible
        '''
        msclt = MySubHandler()
        cond = msclt.setup()

        o = self.opc.get_objects_node()

        # subscribe to a variable
        startv1 = [1, 2, 3]
        v1 = o.add_variable(3, 'SubscriptionVariableV1', startv1)
        sub = self.opc.create_subscription(100, msclt)
        handle1 = sub.subscribe_data_change(v1)

        # Now check we get the start value
        with cond:
            ret = cond.wait(0.5)
        if sys.version_info.major>2: self.assertEqual(ret, True) # we went into timeout waiting for subcsription callback
        else: pass # XXX
        self.assertEqual(msclt.value, startv1)
        self.assertEqual(msclt.node, v1)

        # modify v1 and check we get value 
        v1.set_value([5])
        with cond:
            ret = cond.wait(0.5)
        if sys.version_info.major>2: self.assertEqual(ret, True) # we went into timeout waiting for subcsription callback
        else: pass # XXX
        self.assertEqual(msclt.node, v1)
        self.assertEqual(msclt.value, [5])

        sub.unsubscribe(handle1)
        sub.delete()

    def test_get_node_by_nodeid(self):
        root = self.opc.get_root_node()
        server_time_node = root.get_child(['0:Objects', '0:Server', '0:ServerStatus', '0:CurrentTime'])
        correct = self.opc.get_node(opcua.ObjectId.Server_ServerStatus_CurrentTime)
        self.assertEqual(server_time_node, correct)

    def test_subscribe_server_time(self):
        msclt = MySubHandler()
        cond = msclt.setup()

        server_time_node = self.opc.get_node(opcua.ObjectId.Server_ServerStatus_CurrentTime)

        sub = self.opc.create_subscription(200, msclt)
        handle = sub.subscribe_data_change(server_time_node)

        with cond:
            ret = cond.wait(0.5)
        if sys.version_info.major>2: self.assertEqual(ret, True) # we went into timeout waiting for subcsription callback
        else: pass # XXX
        self.assertEqual(msclt.node, server_time_node)
        # FIXME: Add test to verify server clock. How do I convert opcua.DataTime to python datetime?

        sub.unsubscribe(handle)
        sub.delete()

    def test_datetime_read(self):
        time_node = self.opc.get_node(opcua.ObjectId.Server_ServerStatus_CurrentTime)
        dt = time_node.get_value()
        pydt = dt.to_datetime()
        utcnow = datetime.datetime.utcnow()
        delta = utcnow - pydt
        self.assertTrue(delta < datetime.timedelta(seconds=1))

    def test_datetime_write(self):
        time_node = self.opc.get_node(opcua.ObjectId.Server_ServerStatus_CurrentTime)
        now = datetime.datetime.now()
        objects = self.opc.get_objects_node()
        v1 = objects.add_variable(4, "test_datetime", now)
        tid = v1.get_value()
        self.assertEqual(now, tid.to_datetime())




class ServerProcess(Thread):
    '''
    Start a server in another process
    '''
    def __init__(self):
        Thread.__init__(self)
        self._exit = Event()
        self.started = Event()
        self._queue = Queue()

    def run(self):
        self.srv = opcua.Server()
        self.srv.set_endpoint('opc.tcp://localhost:%d' % port_num1)
        self.srv.start()
        self.started.set()
        while not self._exit.is_set():
            time.sleep(0.1)
            if not self._queue.empty():
                ev = self._queue.get()
                self.srv.trigger_event(ev)
        self.srv.stop()

    def stop(self):
        self._exit.set()

    def trigger_event(self, ev):
        self._queue.put(ev)


class TestClient(unittest.TestCase, CommonTests):
    '''
    Run common tests on client side
    Of course we need a server so we start a server in another 
    process using python Process module
    Tests that can only be run on client side must be defined here
    '''
    @classmethod
    def setUpClass(self):
        # start server in its own process
        global globalserver
        self.srv = globalserver 
        self.srv.start()
        self.srv.started.wait() # let it initialize

        # start client
        self.clt = opcua.Client();
        self.clt.connect('opc.tcp://localhost:%d' % port_num1)
        self.opc = self.clt

    @classmethod
    def tearDownClass(self):
        self.clt.disconnect()
        # stop the server in its own process
        self.srv.stop()
        # wait for server to stop, otherwise we may try to start a 
        # new one before this one is really stopped
        self.srv.join()


class TestServer(unittest.TestCase, CommonTests):
    '''
    Run common tests on server side
    Tests that can only be run on server side must be defined here
    '''
    @classmethod
    def setUpClass(self):
        self.srv = opcua.Server()
        self.srv.set_endpoint('opc.tcp://localhost:%d' % port_num2)
        self.srv.start()
        self.opc = self.srv 

    @classmethod
    def tearDownClass(self):
        self.srv.stop()

    def test_register_namespace(self):
        uri = 'http://mycustom.namespace.com'
        idx1 = self.opc.register_namespace(uri)
        idx2 = self.opc.get_namespace_index(uri)
        self.assertEqual(idx1, idx2) 

    def test_register_use_namespace(self):
        uri = 'http://my_very_custom.namespace.com'
        idx = self.opc.register_namespace(uri)
        root = self.opc.get_root_node()
        myvar = root.add_variable(idx, 'var_in_custom_namespace', [5])
        myid = myvar.get_id()
        self.assertEqual(idx, myid.namespace_index) 
        #self.assertEqual(uri, myid.namespace_uri) #FIXME: should return uri!!!




if __name__ == '__main__':
    globalserver = ServerProcess() #server process will be started by client tests
    try:
        sclt = SubHandler()
        unittest.main(verbosity=3)
    finally:
        globalserver.stop()

