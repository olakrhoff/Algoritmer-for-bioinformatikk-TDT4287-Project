# This code is based on the wonderful explanation in the link below:
# https://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english

class Node:
    def __init__(self, start, end, parent):
        self.start = start  # Index in sequence
        self.end = end  # Index in sequence
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

    def __eq__(self, other):
        return self.length == other.length and self.edge == other.edge and self.node == other.node


class SuffixTree:
    def __init__(self, text):
        self.root = Node(-1, -1, None)  # Create the root node
        self.text: str = text
        #self.num_sequences: int = 0
        self.end_pointer: int = -1
        #self.current_node: Node = self.root
        self.active_point: ActivePoint = ActivePoint(self.root, "", 0)
        #self.reminder: int = 0

    def get_label(self, node: Node):
        if node.is_leaf():
            return self.text[node.start: self.end_pointer]
        return self.text[node.start: node.end]

    def build(self):
        self.root.children.append(Node(0, -1, self.root))  # Add first element to construct I_0

        self.active_point = ActivePoint(self.root, "", 0)
        j_i = 0  # Last suffix that was explicitly added
        for i in range(len(self.text) - 1):
            previous_node: Node = None
            l = self.text[i + 1]
            self.end_pointer = i + 1
            while j_i < self.end_pointer:
                if self.traverse(l):
                    break
                else:
                    new_node: Node = self.add_explicit_point(i + 1)
                    if new_node.is_leaf():
                        self.active_point = ActivePoint(self.root, "", 0)  # Reset
                    else:  # Internal node
                        if not previous_node is None:
                            previous_node.link = new_node
                        previous_node = new_node

                        self.active_point = ActivePoint(self.root, self.text[j_i + self.active_point.length], self.active_point.length - 1)
                    j_i += 1

    def traverse(self, l: str):
        has_path = False
        node: Node = None
        if self.active_point.length > 0:
            for n in self.active_point.node.children:
                if self.text[n.start] == self.active_point.edge:
                    has_path = True
                    node = n
                    break

            if not has_path:
                return False
        else:
            for n in self.active_point.node.children:
                if self.text[n.start] == l:
                    self.active_point.edge = l
                    self.active_point.length = 1
                    return True
            return False

        # We have a valid path to check
        # From here on out length will be ≥ 1

        if self.text[node.start + self.active_point.length] == l:  # Already in the tree
            if len(self.get_label(node)) == self.active_point.length + 1:
                self.active_point = ActivePoint(node, "", 0)
                return True
            else:
                self.active_point.length += 1
                return True
        else:
            return False
            node = self.active_point.node
            if self.get_label(node)[self.active_point.length] == l:  # Next letter along edge matches new letter
                self.active_point.length += 1
                if self.active_point.length == len(self.get_label(node)):
                    self.active_point = ActivePoint(node, "", 0)
                    return True
            return False

    def add_explicit_point(self, index: int):
        if self.active_point.edge == "":
            new_node = Node(index, -1, self.active_point.node)
            self.active_point.node.children.append(new_node)
            return new_node

        node_from: Node = None
        for n in self.active_point.node.children:
            if self.text[n.start] == self.active_point.edge:
                node_from = n
                break
        if node_from is None:
            exit("BAD 1")

        new_node = Node(node_from.start, self.active_point.length - 1, self.active_point.node)
        node_from.start += self.active_point.length
        node_from.parent = new_node
        new_node.children.append(node_from)
        new_node.children.append(Node(index, -1, new_node))

        return new_node
