//Most of this code is based on the wonderful explanation in the link below:
//https://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

typedef struct node
{
    uint32_t start {}, end {}, *end_p {};
    node *parent {nullptr}, *link {nullptr};
    std::vector<node*> children {};
    
    node() = default;
    
    node(uint32_t start, uint32_t anEnd, uint32_t *endP, node *parent, node *link, std::vector<node *> children)
            : start(start), end(anEnd), end_p(endP), parent(parent), link(link), children(std::move(children))
    {}
    
    virtual ~node()
    {
        for (node *val : children)
            delete val;
    }
    
    bool is_leaf() const
    {
        return children.empty(); //No children indicate that it is a leaf node
    }
    
    void add_child(node *a_node)
    {
        children.emplace_back(a_node);
    }
    
} node_t;

typedef struct active_point
{
    node_t* node {nullptr};
    char edge = {*"\0"};
    uint64_t length {0};
    
    active_point() = default;
    
    active_point(node_t* node, char edge, uint64_t length) : node(node), edge(edge), length(length)
    {}
    
    void set_to_node(node_t *a_node)
    {
        node = a_node;
        edge = *"\0";
        length = 0;
    }
    
} active_point_t;

typedef class suffix_tree
{
    private:
        std::string m_text {};
        node_t m_root {};
        uint32_t m_end_pointer {0};
        active_point_t m_active_point {};
        uint32_t m_remainder {0};
        
    public:
        explicit suffix_tree(std::string a_text) : m_text(std::move(a_text))
        {}
        
        void build()
        {
            m_root.add_child(new node_t(0, 0, &m_end_pointer, &m_root, nullptr, {})); //Add a leaf node
            
            m_active_point = active_point_t(&m_root, *"\0", 0);
            
            uint32_t j_i = 0; //Last suffix that was explicitly added
            
            for (int i = 0; i < m_text.length() - 1; ++i)
            {
                char letter = m_text[i + 1];
                m_end_pointer = i + 1;
                ++m_remainder;
                node_t *previous_node {nullptr};
                while (m_remainder > 0)
                {
                    if (traverse(letter))
                        break;
                    else
                    {
                        node_t *new_node = add_explicit_point((uint32_t)m_end_pointer);
                        
                        //Update the active point
                        if (!new_node->is_leaf())
                        {
                            //Rule 1
                            if (m_active_point.node == &m_root)
                            {
                                m_active_point.edge = m_text[i + 1 - (m_remainder - 1) +
                                                             1]; //i + 1 is current last pos, then we go back to the first suffix that aren't explicitly added, then increment to the next one
                                --m_active_point.length;
                            }
                            else //Rule 3
                            {
                                if (m_active_point.node->link != nullptr) //Follow suffix link
                                    m_active_point.node = m_active_point.node->link;
                                else //No suffix link, go to root
                                    m_active_point.node = &m_root;
                            }
                            
                            //Rule 2: If we split an edge, so not add a new leaf
                            if (previous_node != nullptr)
                                previous_node->link = new_node;
                            previous_node = new_node;
                        }
                        --m_remainder;
                    }
                }
            }
        }
        
    private:
        void check_active_point(node_t *a_node)
        {
            if (m_active_point.length == get_label_length(a_node)) //We have traversed the whole edge and update the
                m_active_point.set_to_node(a_node);
        }
        
        bool traverse(char letter)
        {
            if (m_active_point.length == 0) //Active point is at a node
            {
                node_t *path_node {};
                for (node_t* node : m_active_point.node->children)
                    if (m_text[node->start] == letter)
                    {
                        m_active_point.edge = letter;
                        ++m_active_point.length;
                        check_active_point(node);
                        return true; //There is a path in the tree already
                    }
                return false; //There is not a path, we need to create it at active point
            }
            else //Active point is in a label on an edge towards a node
            {
                node_t *to_node {};
                for (node_t* node : m_active_point.node->children)
                    if (m_text[node->start] == m_active_point.edge)
                    {
                        to_node = node;
                        break;
                    }
                if (to_node == nullptr)
                    exit(3); //This is not supposed to happen
                    
                if (m_text[to_node->start + m_active_point.length] == letter) //Letter already in tree along the edge
                {
                    ++m_active_point.length;
                    check_active_point(to_node);
                    return true;
                }
                else //Letter not found, we need to insert internal node
                    return false;
            }
        }
        
        node_t *add_explicit_point(uint32_t index)
        {
            if (m_active_point.length == 0) //Create new node from node
            {
                node_t *new_node = new node_t(index, 0, &m_end_pointer, m_active_point.node, nullptr, {}); //Leaf node
                m_active_point.node->add_child(new_node);
                return new_node;
            }
            else //We need to create a new internal node to split the label
            {
                node_t *to_node {};
                for (node_t* node : m_active_point.node->children)
                    if (m_text[node->start] == m_active_point.edge)
                    {
                        to_node = node;
                        break;
                    }
                if (to_node == nullptr)
                    exit(4); //This is not supposed to happen, this is the node that is at the end of the edge we want to split
                
                node_t *new_node = new node_t(to_node->start, to_node->start + m_active_point.length - 1, nullptr, m_active_point.node, nullptr, {to_node});
                node_t *leaf = new node_t(index, 0, &m_end_pointer, new_node, nullptr, {});
                new_node->add_child(leaf);
                to_node->start += m_active_point.length;
                to_node->parent = new_node;
                
                for (int i = 0; i < m_active_point.node->children.size(); ++i)
                    if (m_active_point.node->children.at(i) == to_node)
                    {
                        m_active_point.node->children.at(i) = new_node;
                        break;
                    }
                    
                
                return new_node;
            }
        }
        
        inline std::string get_label(node_t *a_node)
        {
            if (a_node->is_leaf())
                return m_text.substr(a_node->start, *a_node->end_p - a_node->start);
            return m_text.substr(a_node->start, a_node->end - a_node->start);
        }
        
        inline static uint32_t get_label_length(node_t *a_node)
        {
            if (a_node->is_leaf())
                return *a_node->end_p - a_node->start + 1;
            return a_node->end - a_node->start + 1;
        }
        
} suffix_tree_t;

void task1(std::string primer, std::string filepath)
{
    std::ifstream file_descriptor;
    file_descriptor.open(filepath);
    
    if (!file_descriptor.is_open())
        exit(EXIT_FAILURE);
    
    //suffix_tree_t tree(primer);
    suffix_tree_t tree("abcabxabcd");
    
    tree.build();
    
    file_descriptor.close();
}

namespace fs = std::filesystem;

int main()
{
    std::string a = "TGGAATTCTCGGGTGCCAAGGAACTCCAGTCACACAGTGATCTCGTATGCCGTCTTCTGCTTG";
    
    task1(a, "../../../data/s_3_sequence_1M.txt");
    
    
    
    
    return EXIT_SUCCESS;
}
