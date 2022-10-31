
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include "keyword_tree.cpp"

#include <sys/time.h>
#define WALLTIME(t) ((double)(t).tv_sec + 1e-6 * (double)(t).tv_usec)

struct timeval
        t_start,
        t_stop;
double
t_total;

void task4(std::string filepath, std::string name, double certainty)
{
    keyword_tree_t tree(*"$");
    std::ifstream fd_in;
    fd_in.open(filepath);
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
        
        tree.add_sequence(sequence);
    }
    fd_in.close();
    std::cout << number_of_sequences << std::endl;
    std::cout << "Dataset done" << std::endl;
    
    std::cout << "Finding most probable adapter..." << std::endl;
    std::string probable_adapter = tree.get_probable_adapter(certainty); //AGATCGGAAGAGCACACGTCTGAACTCCAGTCACGTAGAGATCTCGTATGCCGTCTTCTGCTTGAA
    
    std::cout << "The most probable adapter is: \"" << probable_adapter << "\"" << std::endl;
    
    //Generate the length distribution of the sequences after removing found adapter
    fd_in.open(filepath);
    if (!fd_in.is_open())
        exit(56);
    sequence = "";
    std::vector<uint32_t> length_dist{};
    std::cout << "Generating length distribution after removing adapter..." << std::endl;
    while (std::getline(fd_in, sequence))
    {
        if (sequence.empty())
            break; //We are done
        
        if (sequence.length() < probable_adapter.length())
            continue; //a perfect match or complete overlap
        uint32_t length = sequence.length() - probable_adapter.length();
        while (length_dist.size() <= length)
            length_dist.emplace_back(0);
        ++length_dist[length];
    }
    fd_in.close();
    
    std::cout << "Writing length distribution after removing adapter to file..." << std::endl;
    std::ofstream fd_out;
    fd_out.open("data/task4_data_len_dist_" + name + ".csv");
    if (!fd_out.is_open())
        exit(77);
    
    for (int i = 0; i < length_dist.size(); ++i)
    {
        fd_out << length_dist[i];
        if (i < length_dist.size() - 1)
            fd_out << ",";
    }
    fd_out.close();
    
    //Generate set of sequences without the adapter in keyword tree
    fd_in.open(filepath);
    if (!fd_in.is_open())
        exit(56);
    sequence = "";
    keyword_tree_t tree_without_adapter(*"$");
    std::cout << "Creating keyword tree for sequences without adapter..." << std::endl;
    while (std::getline(fd_in, sequence))
    {
        if (sequence.empty())
            break; //We are done
    
        if (sequence.length() < probable_adapter.length())
            continue; //a perfect match or complete overlap
        sequence = sequence.substr(0, sequence.length() - probable_adapter.length());
        
        tree_without_adapter.add_sequence(sequence);
    }
    fd_in.close();
    
    std::cout << "Finding most common suffix..." << std::endl;
    std::string probable_common_suffix = tree_without_adapter.get_probable_adapter(0.5); //
    
    std::cout << "The most probable common suffix is: \"" << probable_common_suffix << "\"" << std::endl;
    
    
    //Finding unique sequences and their frequency distribution
    std::vector<uint32_t> frequency_dist {};
    std::vector<node_t *> stack {};
    stack.push_back(tree_without_adapter.get_root()); //Important that this is not changed since the adapter was found
    
    std::cout << "Finding unique sequence frequency distribution" <<std::endl;
    while (!stack.empty())
    {
        node_t *current_node = stack.back();
        stack.pop_back();
        for (node_t *child : current_node->children)
            stack.push_back(child);
        if (current_node->is_leaf())
            frequency_dist.emplace_back(current_node->passes);
    }
    
    std::cout << "Writing frequency distribution to file..." << std::endl;
    fd_out.open("data/task4_data_unique_freq_dist_" + name + ".csv");
    if (!fd_out.is_open())
        exit(78);
    
    for (int i = 0; i < frequency_dist.size(); ++i)
    {
        fd_out << frequency_dist[i];
        if (i < frequency_dist.size() - 1)
            fd_out << ",";
    }
    fd_out.close();
    
}


int main()
{
    std::string tdt_file = "data/tdt4287-unknown-adapter.txt";
    std::string s_3_file = "data/s_3_sequence_1M.txt";
    std::string seq_file = "data/seqset3.txt";
    std::cout << "Starting task 4" << std::endl;
 
    gettimeofday(&t_start, NULL);
    
    
    task4(tdt_file, "tdt", 0.7);
    
    gettimeofday(&t_stop, NULL);
    t_total = WALLTIME(t_stop) - WALLTIME(t_start);
    printf("%.2lf seconds total runtime\n", t_total);
    
    std::cout << "\n\n--------------------------------\n\n";
    task4(s_3_file, "s_3", 0.5);
    std::cout << "\n\n--------------------------------\n\n";
    task4(seq_file, "seq", 0.5);
    std::cout << "Complete" << std::endl;
    return EXIT_SUCCESS;
}
