//Most of this code is based on the wonderful explanation in the link below:
//https://stackoverflow.com/questions/9452701/ukkonens-suffix-tree-algorithm-in-plain-english

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

#include <sys/time.h>
#define WALLTIME(t) ((double)(t).tv_sec + 1e-6 * (double)(t).tv_usec)

struct timeval
        t_start,
        t_stop;
double
    t_total;

typedef struct node
{
    uint32_t start {}, end {}, *end_p {}, string_id {};
    node *parent {nullptr}, *link {nullptr};
    std::vector<node*> children {};
    
    node() = default;
    
    node(uint32_t start, uint32_t anEnd, uint32_t *endP, uint32_t stringId, node *parent, node *link,
         std::vector<node *> children) : start(start), end(anEnd), end_p(endP), string_id(stringId),
                                                parent(parent), link(link), children(std::move(children))
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
    
    std::string print() const
    {
        std::string printed = to_string();
        for (node *child : children)
            printed += child->print();
        return printed + "\n";
    }
    
    std::string to_string() const
    {
        std::string val = "Node: " + std::to_string((uint64_t) this) + "\nStart: " + std::to_string(start) + " End: ";
        
        if (is_leaf())
            val += std::to_string(*end_p);
        else
            val += std::to_string(end);
        
        return val + " String: " + std::to_string(string_id) + " Leaf?: " + std::to_string(is_leaf()) + " Children: " + std::to_string(children.size()) + "\n";
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
        uint32_t *m_end_pointers {nullptr};
        active_point_t m_active_point {};
        uint32_t m_remainder {0};
        char m_termination_char {*"$"};
        uint32_t m_num_sequence {0};
        uint32_t m_j_i {0};
        
    public:
        suffix_tree(std::string a_text, char a_termination_char) : m_text(std::move(a_text)), m_termination_char(a_termination_char)
        {
            m_end_pointers = new uint32_t[std::count(a_text.begin(), a_text.end(), a_termination_char)];
        }
        
        virtual ~suffix_tree()
        {
            delete m_end_pointers;
        }
        
        void build()
        {
            m_root.add_child(new node_t(0, 0, &m_end_pointers[m_num_sequence], m_num_sequence, &m_root, nullptr, {})); //Add a leaf node
            
            m_active_point = active_point_t(&m_root, *"\0", 0);
            
            for (int i = 0; i < m_text.length() - 1; ++i)
            {
                char letter = m_text[i + 1];
                m_end_pointers[m_num_sequence] = i + 1;
                ++m_remainder;
                node_t *previous_node {nullptr};
                while (m_remainder > 0)
                {
                    if (traverse(letter))
                        break;
                    else
                    {
                        node_t *new_node = add_explicit_point(m_end_pointers[m_num_sequence]);
                        
                        if (m_active_point.node == &m_root && m_active_point.length > 0)
                        {
                            --m_active_point.length;
                            m_active_point.edge = m_text[i + 1 - (m_remainder - 1) + 1];
                        }
                        if (!new_node->is_leaf() || new_node->parent != &m_root) //internal node
                        {
                            if (previous_node != nullptr)
                                previous_node->link = new_node;
                            previous_node = new_node;
                        }
                        if (m_active_point.node != &m_root)
                        {
                            m_active_point.node = m_active_point.node->link != nullptr ? m_active_point.node->link : &m_root;
                        }
                        /*
                        //Update the active point
                        if (!new_node->is_leaf())
                        {
                            //Rule 1
                            if (m_active_point.node == &m_root)
                            {
                                m_active_point.edge = m_text[i + 1 - (m_remainder - 1) +
                                                             1]; //i + 1 is current last pos, then we go back to the first suffix that aren't explicitly added, then increment to the next one
                                --m_active_point.length;
                                check_active_point();
                            }
                            else //Rule 3
                            {
                                if (m_active_point.node->link != nullptr) //Follow suffix link
                                    m_active_point.node = m_active_point.node->link;
                                else //No suffix link, go to root
                                    m_active_point.node = &m_root;
                                check_active_point();
                            }
                            
                            //Rule 2: If we split an edge, so not add a new leaf
                            if (previous_node != nullptr)
                                previous_node->link = new_node;
                            previous_node = new_node;
                        }
                        else if (m_active_point.node != &m_root) //If we created a leaf node from an internal node
                        {
                            node_t *node = m_active_point.node;
                            m_active_point.node = node->parent;
                            m_active_point.edge = m_text[node->start + 1];
                            m_active_point.length = get_label_length(node) - 1;
                            
                            check_active_point();
                        }
                         */
                        --m_remainder;
                    }
                }
                
                if (letter == m_termination_char) //We are finished with one string
                    ++m_num_sequence;
            }
        }
        
        uint32_t prefix_suffix_match(int prefix_id, int suffix_id)
        {
            node_t* current_node = &m_root;
            uint32_t index {0};
            if (prefix_id > 0)
                index = m_end_pointers[prefix_id - 1] + 1;
            while (!current_node->is_leaf())
            {
                for (node_t *node : current_node->children)
                    if (m_text[node->start] == m_text[index])
                    {
                        current_node = node;
                        index += get_label_length(node);
                        break;
                    }
            }
            
            //We should now be at the leaf representing the prefix string
            
            while (current_node->parent != nullptr)
            {
                std::vector<node_t*> stack {current_node->parent};
                bool has_suffix {false};
                while (!stack.empty())
                {
                    node_t *node = stack.back();
                    stack.pop_back();
                    if (!node->is_leaf())
                        for (node_t *n : node->children)
                            stack.push_back(n);
                    else if (node->string_id == suffix_id)
                    {
                        has_suffix = true;
                        break;
                    }
                }
                if (has_suffix)
                    break;
                current_node = current_node->parent;
            }
            
            uint32_t suffix {0};
            
            while (current_node->parent != &m_root)
            {
                current_node = current_node->parent;
                suffix += get_label_length(current_node);
            }
            
            return suffix;
        }
        
        void construct(std::string data)
        {
            //Construct I_1
            m_root.add_child(new node_t(0, 0, &m_end_pointers[m_num_sequence], m_num_sequence, &m_root, nullptr, {})); //Add a leaf node
            m_text += data.at(0);
            m_active_point = active_point_t(&m_root, *"\0", 0);
            for (int i = 0; i < data.length() - 1; ++i)
            {
                m_text += data.at(i + 1);
                char letter = m_text[i + 1];
                extend(letter);
            }
        }
        
        void extend(char letter)
        {
            uint32_t i = m_text.length() - 2;
            m_end_pointers[m_num_sequence] = i + 1;
            //for (int j = m_j_i + 1; j <= i + 1; ++j)
            //{
                if (traverse(letter))
                    //break;
                    return;
                else
                {
                    sea();
                    //j = m_j_i;
                }
            //}
            if (letter == m_termination_char) //We are finished with one string
                ++m_num_sequence;
        }
        
    private:
        void sea()
        {
            node_t *previous_node {nullptr};
            node_t *new_node = add_explicit_point(m_end_pointers[m_num_sequence]);
            ++m_j_i;
            
            if (!new_node->is_leaf() || new_node->parent != &m_root)
            {
                previous_node = new_node;
            }
            
            char letter = m_text[m_end_pointers[m_num_sequence]];
            while (true)
            {
                if (m_active_point.node == &m_root && m_active_point.length == 0)
                    return;
                else if (m_active_point.length == 0)
                {
                    m_active_point.length = get_label_length(m_active_point.node);
                    m_active_point.edge = m_text[m_active_point.node->start];
                    m_active_point.node = m_active_point.node->parent;
                }
                
                node_t *node_to {nullptr};
                for (node_t *n: m_active_point.node->children)
                    if (m_text[n->start] == m_active_point.edge)
                    {
                        node_to = n;
                        break;
                    }
                if (node_to == nullptr)
                    exit(9); //Not good
    
                if (get_label_length(node_to) < m_active_point.length)
                    exit(10); //Bad
    
                node_t *v = m_active_point.node;
                std::string gamma = m_text.substr(node_to->start, m_active_point.length);
    
                if (!v->is_leaf() && v != &m_root &&
                    v->link != nullptr) //Checking that v is an internal node with suffix link
                {
                    if (traverse(m_text[m_end_pointers[m_num_sequence]]))
                    {
                        m_active_point.set_to_node(&m_root);
                        return;
                    }
                    
                    walk(v->link, gamma);
                    
                    node_t *new_node = add_explicit_point(m_end_pointers[m_num_sequence]);
                    ++m_j_i;
                    
                    if (!new_node->is_leaf() || new_node->parent != &m_root)
                    {
                        if (previous_node != nullptr)
                            previous_node->link = new_node;
                        previous_node = new_node;
                    }
                    continue;
                }
                else
                {
                    if (traverse(m_text[m_end_pointers[m_num_sequence]]))
                    {
                        m_active_point.set_to_node(&m_root);
                        return;
                    }
                    
                    std::string path = m_text.substr(m_j_i + 2, m_text.length() - (m_j_i + 1) - 1);
                    walk(&m_root, path);
                    
                    node_t *new_node = add_explicit_point(m_end_pointers[m_num_sequence]);
                    ++m_j_i;
                    
                    if (!new_node->is_leaf() || new_node->parent != &m_root)
                    {
                        if (previous_node != nullptr)
                            previous_node->link = new_node;
                        previous_node = new_node;
                    }
                    
                    continue;
                }
                
                //If rule 3: Path already here; end
                return;
            }
        }
        
        void walk(node_t *start_node, std::string path)
        {
            /*if (path == "")
            {
                m_active_point.node = start_node;
                return;
            }*/
            uint32_t h {0};
            while (true)
            {
                if (h == path.length())
                {
                    m_active_point.set_to_node(start_node);
                    return;
                }
                node_t *node_to {nullptr};
                for (node_t *n: start_node->children)
                    if (m_text[n->start] == path.at(h))
                    {
                        node_to = n;
                        break;
                    }
                if (node_to == nullptr)
                    exit(11); //Not good
                    
                if (get_label_length(node_to) > path.length())
                {
                    m_active_point = active_point_t(start_node, path.at(h), path.length() - h);
                    return;
                }
                else
                {
                    start_node = node_to;
                    h += get_label_length(node_to);
                }
            }
        }
        
        void check_active_point()
        {
            if (m_active_point.length == 0)
            {
                if (m_active_point.node == &m_root)
                    return; //This is fine, we are in the root
                
                //If we are not in the root we need to decrement the value
                node_t *node = m_active_point.node;
                m_active_point.node = node->parent;
                m_active_point.edge = m_text[node->start + 1];
                m_active_point.length = get_label_length(node) - 1;
                return;
            }
            node_t *to_node {};
            for (node_t *node : m_active_point.node->children)
                if (m_text[node->start] == m_active_point.edge)
                {
                    to_node = node;
                    break;
                }
            if (to_node == nullptr)
                exit(6); //Bad
            
            if (m_active_point.length == get_label_length(to_node)) //We have traversed the whole edge and update the
                m_active_point.set_to_node(to_node);
            else if (m_active_point.length > get_label_length(to_node))
            {
                m_active_point.node = to_node;
                m_active_point.length -= get_label_length(to_node);
                m_active_point.edge = m_text[to_node->start]; //This need to be set to the next
            }
            
            
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
                node_t *new_node = new node_t(index, 0, &m_end_pointers[m_num_sequence], m_num_sequence, m_active_point.node, nullptr, {}); //Leaf node
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
                
                node_t *new_node = new node_t(to_node->start, to_node->start + m_active_point.length - 1, nullptr, m_num_sequence, m_active_point.node, nullptr, {to_node});
                
                if (new_node->start > new_node->end)
                    exit(7); //Not valid
                
                node_t *leaf = new node_t(index, 0, &m_end_pointers[m_num_sequence], m_num_sequence, new_node, nullptr, {});
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
        
        std::string print() const
        {
            return m_root.print();
        }
    
    public:
        friend std::ostream &operator<<(std::ostream &os, const suffix_tree &tree)
        {
            os << "End points: ";
            for (int i = 0; i < tree.m_num_sequence; ++i )
                os << std::to_string(tree.m_end_pointers[i]) + " ";
            os << "\n";
            return os << tree.print();
        }
} suffix_tree_t;


void task1(const std::string primer, std::string filepath)
{
    std::ifstream in_fd;
    in_fd.open(filepath);
    
    if (!in_fd.is_open())
        exit(44);
    std::string sequence;
    std::vector<uint32_t> length_of_matching_sequences {};
    uint64_t number_of_sequences {0};
    while (std::getline(in_fd, sequence))
    {
        if (sequence.empty())
            exit(5); //This should not happen
        ++number_of_sequences;
        
        bool match = false;
        uint32_t offset {0};
        for (int i = 0; i < sequence.length(); ++i)
        {
            char s = sequence.at(i);
            char p = primer.at(offset);
            if (sequence.at(i) == primer.at(offset))
            {
                ++offset;
                match = true;
            }
            else
            {
                offset = 0;
                match = false;
            }
        }
        if (!match)
            continue;
        
        uint64_t suffix = sequence.length() - offset;
        while (length_of_matching_sequences.size() <= suffix)
            length_of_matching_sequences.emplace_back(0);
        ++length_of_matching_sequences.at(suffix); //Increment the observation of prefix suffix match length
    }
    in_fd.close();
    
    std::ofstream  out_fd;
    out_fd.open("data/task1_data.csv");
    
    if (!out_fd.is_open())
        exit(45);
    
    for (int i = 0; i < length_of_matching_sequences.size(); ++i)
    {
        out_fd << length_of_matching_sequences[i];
        out_fd << ",";
    }
    out_fd << number_of_sequences;
    out_fd.close();
    std::cout << "Number of sequences: " << number_of_sequences << std::endl;
}

int main()
{
    std::string a = "TGGAATTCTCGGGTGCCAAGGAACTCCAGTCACACAGTGATCTCGTATGCCGTCTTCTGCTTG";
    
    gettimeofday(&t_start, NULL);
    
    task1(a, "data/s_3_sequence_1M.txt");
    
    gettimeofday(&t_stop, NULL);
    t_total = WALLTIME(t_stop) - WALLTIME(t_start);
    printf("%.2lf seconds total runtime\n", t_total);
    
    return EXIT_SUCCESS;
}
