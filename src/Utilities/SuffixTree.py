# This code is based on the wonderful explanation in the link below:
# https://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english

class Node:
    def __init__(self, start, end, parent):
        self.start = start  # Index in sequence
        self.end = end # Index in sequence
        self.parent: Node = parent
        self.children = []
        self.link: Node = None

    def is_leaf(self):
        return len(self.children) == 0

class ActivePoint:
    def __init__(self, node, edge, length):
        self.node: Node = node
        self.edge: str = edge
        self.length: int = length

class SuffixTree:
    def __init__(self, text):
        self.root = Node(-1, -1, None)  # Create the root node
        self.text: str = text
        self.num_sequences: int = 0
        self.end_pointer: int = -1
        self.current_node: Node = self.root
        self.active_point: ActivePoint = ActivePoint(self.root, "", 0)
        self.reminder: int = 0

    def get_label(self, node: Node):
        if node.is_leaf():
            return self.text[node.start : self.end_pointer]
        return self.text[node.start : node.end]

    def construct(self):

        self.root.children.append(Node(0, -1, self.root))  # Add first element to construct I_0

        j_i = 0
        for i in range(1, len(self.text) - 1):
            self.end_pointer = i + 1
            for j in range(j_i + 1, i + 1):
                suffix = self.text[j:i + 1]
                for node in self.root.children:
                    if self.text[node.start] == suffix[0]:
                        label = self.get_label(node)
                        for l in range(1, len(label)):
                            if label[l] != suffix[l]:





    def build(self):

        self.root.children.append(Node(0, -1, self.root))  # Add first element to construct I_0

        j_i = 0  # Last suffix that was explicitly added
        current_j = 1
        for i in range(len(self.text) - 1):
            self.end_pointer = i + 1
            for j in range(j_i + 1, i + 1):
                rule3 = self.SEA(j, i)
                if rule3:
                    break
                current_j = j
            j_i = j - 1

    def SEA(self, j, i):


        return True
