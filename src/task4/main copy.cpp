
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

typedef struct node
{
    uint32_t start {}, end {}, string_id {};
    uint64_t passes {0};
    node *parent {nullptr};
    std::vector<node*> children {};
    
    node() = default;
    
    node(uint32_t start, uint32_t anEnd, uint32_t stringId, uint64_t passes, node *parent,
         std::vector<node *> children) : start(start), end(anEnd), string_id(stringId), passes(passes),
                                                parent(parent), children(std::move(children))
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


typedef struct light_string
{
    char *data {};
    int size {};
    
    light_string(std::string value)
    {
        size = value.length();
        data = new char[size + 1];
        memcpy(data, value.data(), size);
        data[size] = *"\n";
    }
    
    virtual ~light_string()
    {
        delete[] data;
    }
    
    char operator[](int i) const
    {
        return data[i];
    }
    
    int length() const
    {
        return size;
    }
    
    std::string substr(uint32_t pos, uint32_t n)
    {
        return std::string(data + pos, data + pos + n);
    }
} light_string_t;

typedef class suffix_tree
{
    private:
        std::vector<light_string_t *> m_texts {};
        node_t m_root {};
        uint32_t *m_end_pointers {nullptr};
        active_point_t m_active_point {};
        uint32_t m_remainder {0};
        char m_termination_char {*"$"};
        uint32_t m_num_sequence {0};
        uint32_t m_j_i {0};
    
    public:
        suffix_tree(char a_termination_char) : m_termination_char(a_termination_char)
        {
        }
        
        virtual ~suffix_tree()
        {
            delete m_end_pointers;
        }
        
    
    private:
        
        bool traverse(char letter)
        {
            if (m_active_point.length == 0) //Active point is at a node
            {
                node_t *path_node {};
                for (node_t* node : m_active_point.node->children)
                    if ((*m_texts[node->string_id])[node->start] == letter)
                    {
                        m_active_point.edge = letter;
                        ++m_active_point.length;
                        
                        if (m_active_point.length == get_label_length(node))
                        {
                            if (node->is_leaf())
                                m_active_point.set_to_node(&m_root);
                            else
                                m_active_point.set_to_node(node);
                        }
                        
                        return true; //There is a path in the tree already
                    }
                return false; //There is not a path, we need to create it at active point
            }
            else //Active point is in a label on an edge towards a node
            {
                node_t *to_node {};
                for (node_t* node : m_active_point.node->children)
                    if ((*m_texts[node->string_id])[node->start] == m_active_point.edge)
                    {
                        to_node = node;
                        break;
                    }
                if (to_node == nullptr)
                    exit(3); //This is not supposed to happen
                
                if ((*m_texts[to_node->string_id])[to_node->start + m_active_point.length] == letter) //Letter already in tree along the edge
                {
                    ++m_active_point.length;
                    
                    if (m_active_point.length == get_label_length(to_node))
                        m_active_point.set_to_node(to_node);
                    
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
                node_t *new_node = new node_t(index, m_texts.back()->length() - 1, m_texts.size() - 1, 0, m_active_point.node, {}); //Leaf node
                m_active_point.node->add_child(new_node);
                return new_node;
            }
            else //We need to create a new internal node to split the label
            {
                node_t *to_node {};
                for (node_t* node : m_active_point.node->children)
                    if ((*m_texts[node->string_id])[node->start] == m_active_point.edge)
                    {
                        to_node = node;
                        break;
                    }
                if (to_node == nullptr)
                    exit(4); //This is not supposed to happen, this is the node that is at the end of the edge we want to split
                
                node_t *new_node = new node_t(to_node->start, to_node->start + m_active_point.length - 1, to_node->passes, m_num_sequence, m_active_point.node, {to_node});
                
                if (new_node->start > new_node->end)
                    exit(7); //Not valid
                
                node_t *leaf = new node_t(index, m_texts.back()->length() - 1, 0, m_texts.size() - 1, new_node, {});
                new_node->add_child(leaf);
                to_node->start += m_active_point.length;
                to_node->parent = new_node;
                
                for (int i = 0; i < m_active_point.node->children.size(); ++i)
                    if (m_active_point.node->children[i] == to_node)
                    {
                        m_active_point.node->children[i] = new_node;
                        break;
                    }
                
                
                return new_node;
            }
        }
        
        inline std::string get_label(node_t *a_node)
        {
            return m_texts[a_node->string_id]->substr(a_node->start, a_node->end - a_node->start);
        }
        
        inline static uint32_t get_label_length(node_t *a_node)
        {
            return a_node->end - a_node->start + 1;
        }
    
    public:
        
        void add_sequence(std::string a_sequence)
        {
            std::reverse(a_sequence.begin(), a_sequence.end());
            m_texts.emplace_back(new light_string_t(a_sequence + m_termination_char));
            m_active_point.set_to_node(&m_root);
            for (int i = 0; i < m_texts.back()->length(); ++i)
            {
                if (traverse((*m_texts.back())[i]))
                    continue;
                
                //If we dont have a match, create new node with the rest of the string
                add_explicit_point(i);
                break;
            }
        }
        
        std::string get_probable_adapter()
        {
            m_active_point.set_to_node(&m_root);
            std::string adapter {};
            while (!m_active_point.node->is_leaf())
            {
                for (node_t *child : m_active_point.node->children)
                {
                    double percent = (double)child->passes / (double)m_active_point.node->passes;
                    if (percent >= 0.5)
                    {
                        m_active_point.set_to_node(child);
                        adapter += get_label(child);
                    }
                }
            }
            std::reverse(adapter.begin(), adapter.end());
            
            return adapter;
        }
} suffix_tree_t;

void task4()
{
    suffix_tree_t tree(*"$");
    std::ifstream fd_in;
    fd_in.open("../../../data/tdt4287-unknown-adapter.txt");
    if (!fd_in.is_open())
        exit(55);
    // Number of sequences in set 1957364
    std::string sequence {};
    uint64_t number_of_sequences {0};
    std::cout << "Running dataset..." << std::endl;
    while (std::getline(fd_in, sequence))
    {
        if (sequence.empty())
            break; //We are done
        
        ++number_of_sequences;
        if (number_of_sequences % 10000 == 0)
            std::cout << number_of_sequences << std::endl;
        
        tree.add_sequence(sequence);
    }
    fd_in.close();
    std::cout << number_of_sequences << std::endl;
    std::cout << "Dataset done" << std::endl;
    
    std::string probable_adapter = tree.get_probable_adapter();
    
    std::cout << "The most probable primer is: \"" << probable_adapter << "\"" << std::endl;
}


int main()
{
    task4();
    
    return EXIT_SUCCESS;
}
