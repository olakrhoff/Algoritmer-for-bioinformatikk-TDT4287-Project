#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

typedef struct node
{
    //uint32_t string_id {};
    std::string label {};
    uint64_t passes {0};
    node *parent {nullptr};
    std::vector<node*> children {};
    
    node() = default;
    
    node(std::string label, uint64_t passes, node *parent,
         std::vector<node *> children) : label(std::move(label)), passes(passes), parent(parent),
         children(std::move(children))
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
    
    void set_to_node(node_t *a_node, bool count)
    {
        node = a_node;
        edge = *"\0";
        length = 0;
        if (count)
            node->passes++;
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
        //std::vector<light_string_t *> m_texts {};
        std::string current_seqence {};
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
                    //if ((*m_texts[node->string_id])[node->start] == letter)
                    if (node->label[0] == letter)
                    {
                        m_active_point.edge = letter;
                        ++m_active_point.length;
                        
                        if (m_active_point.length == get_label_length(node))
                        {
                            m_active_point.set_to_node(node, true);
                        }
                        
                        return true; //There is a path in the tree already
                    }
                return false; //There is not a path, we need to create it at active point
                }
            else //Active point is in a label on an edge towards a node
            {
                node_t *to_node {};
                for (node_t* node : m_active_point.node->children)
                    //if ((*m_texts[node->string_id])[node->start] == m_active_point.edge)
                    if (node->label[0] == m_active_point.edge)
                    {
                        to_node = node;
                        break;
                    }
                if (to_node == nullptr)
                    exit(3); //This is not supposed to happen
                
                    //if ((*m_texts[to_node->string_id])[to_node->start + m_active_point.length] == letter) //Letter already in tree along the edge
                    if (to_node->label[m_active_point.length] == letter)
                    {
                        ++m_active_point.length;
                    
                        if (m_active_point.length == get_label_length(to_node))
                            m_active_point.set_to_node(to_node, true);
                    
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
                //node_t *new_node = new node_t(index, m_texts.back()->length() - 1, m_texts.size() - 1, 0, m_active_point.node, {}); //Leaf node
                node_t *new_node = new node_t(current_seqence.substr(index), 1, m_active_point.node, {}); //Leaf node
                m_active_point.node->add_child(new_node);
                return new_node;
                }
            else //We need to create a new internal node to split the label
            {
                node_t *to_node {};
                for (node_t* node : m_active_point.node->children)
                    //if ((*m_texts[node->string_id])[node->start] == m_active_point.edge)
                    if (node->label[0] == m_active_point.edge)
                    {
                        to_node = node;
                        break;
                    }
                if (to_node == nullptr)
                    exit(4); //This is not supposed to happen, this is the node that is at the end of the edge we want to split
                
                    //The internal node
                    //node_t *new_node = new node_t(to_node->start, to_node->start + m_active_point.length - 1, to_node->passes, m_num_sequence, m_active_point.node, {to_node});
                    node_t *new_node = new node_t(to_node->label.substr(0, m_active_point.length), to_node->passes + 1, m_active_point.node, {to_node});
                
                    //if (new_node->start > new_node->end)
                    //exit(7); //Not valid
                
                    //node_t *leaf = new node_t(index, m_texts.back()->length() - 1, 0, m_texts.size() - 1, new_node, {});
                    node_t *leaf = new node_t(current_seqence.substr(index), 1, new_node, {});
                    new_node->add_child(leaf);
                    //to_node->start += m_active_point.length;
                    to_node->label = to_node->label.substr(m_active_point.length);
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
            //return m_texts[a_node->string_id]->substr(a_node->start, a_node->end - a_node->start);
            return a_node->label;
        }
        
        inline static uint32_t get_label_length(node_t *a_node)
        {
            //return a_node->end - a_node->start + 1;
            return a_node->label.length();
        }
    
    public:
        
        void add_sequence(std::string a_sequence)
        {
            std::reverse(a_sequence.begin(), a_sequence.end());
            //m_texts.emplace_back(new light_string_t(a_sequence + m_termination_char));
            current_seqence = std::string(a_sequence + m_termination_char);
            m_active_point.set_to_node(&m_root, true);
            //for (int i = 0; i < m_texts.back()->length(); ++i)
            for (int i = 0; i < current_seqence.length(); ++i)
            {
                //if (traverse((*m_texts.back())[i]))
                if (traverse(current_seqence[i]))
                    continue;
                
                //If we don't have a match, create new node with the rest of the string
                add_explicit_point(i);
                break;
            }
        }
        
        std::string get_probable_adapter(double certainty)
        {
            m_active_point.set_to_node(&m_root, false);
            std::string adapter {};
            while (!m_active_point.node->is_leaf())
            {
                bool sequences_reached = true;
                for (node_t *child : m_active_point.node->children)
                {
                    double percent = (double)child->passes / (double)m_active_point.node->passes;
                    if (percent >= certainty)
                    {
                        m_active_point.set_to_node(child, false);
                        adapter += get_label(child);
                        sequences_reached = false;
                        break;
                    }
                }
                if (sequences_reached)
                    break;
            }
            std::reverse(adapter.begin(), adapter.end());
            
            return adapter;
        }
        
        node_t *get_root()
        {
            return &m_root;
        }
        
        node_t *get_active_node()
        {
            return m_active_point.node;
        }
} keyword_tree_t;
