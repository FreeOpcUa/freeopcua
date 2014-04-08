from libopcua import *
import libopcua

class Node(libopcua.Node):
    def __init__(self, server, nodeid):
        self.server = server
        nodeid = nodeid

    def browse(self):
        return NodeVector(Node.browse(self))
    
    def get_child(self, path):
        return Node.get_child(StringVector(self, path))


if __name__ == "__main__":
    pass
