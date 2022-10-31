#include <iostream>
#include <map>
#include <list>
#include "../task4/keyword_tree.cpp"

#include <sys/time.h>
#define WALLTIME(t) ((double)(t).tv_sec + 1e-6 * (double)(t).tv_usec)

struct timeval
        t_start,
        t_stop;
double
t_total;

uint32_t hamming_distance(const std::string& reference, const std::string& potential_match, uint32_t bound)
{
    if (reference.length() != potential_match.length())
        exit(123); //The lengths must be equal, just for practicality for now at least
    uint32_t distance {};
    for (int i = 0; i < reference.length(); ++i)
        if (reference[i] != potential_match[i])
        {
            ++distance;
            if (distance >= bound)
                return distance;
        }
        
    return distance;
}

void task5(std::string filepath, double certainty, std::string write_dir)
{
    //STEP 1.1: Find the adapter
    keyword_tree_t tree(*"$");
    std::ifstream fd_in;
    fd_in.open(filepath);
    if (!fd_in.is_open())
        exit(55);
    
    //TODO: If the performance of this becomes a bottleneck, try switching the
    //      std::strings in the keyword tree nodes, with the light_string_t type in the same file
    std::string sequence {};
    uint64_t number_of_sequences {0};
    std::cout << "Constructing keyword tree..." << std::endl;
    while (std::getline(fd_in, sequence))
    {
        if (sequence.empty())
            break; //We are done
            ++number_of_sequences; //10687775
        sequence = sequence.substr(0, sequence.length() - 5); //Roughly removing filling
        tree.add_sequence(sequence);
    }
    fd_in.close();
    std::cout << number_of_sequences << std::endl;
    std::cout << "Keyword tree done" << std::endl;
    
    std::cout << "Finding most probable adapter..." << std::endl;
    std::string probable_adapter = tree.get_probable_adapter(certainty);
    
    std::cout << "The most probable adapter is: \"" << probable_adapter << "\"" << std::endl;
    
    //STEP 1.2: Remove the adapter from the sequences
    //Only use sequences that has at most two mismatches and always at least twice
    //as many matches as mismatches and a minimum of six matches
    fd_in.open(filepath);
    if (!fd_in.is_open())
        exit(56);
    sequence = "";
    std::vector<std::string> adapterless_sequences {};
    std::cout << "Aligning and removing adapter from sequences..." << std::endl;
    while (std::getline(fd_in, sequence))
    {
        if (sequence.empty())
            break; //We are done
        
        if (sequence.length() < probable_adapter.length())
            continue; //a perfect match or complete overlap
        
        //TODO: Find the number of filling sequences dynamically, NOT IMPORTANT NOW!
        sequence = sequence.substr(0, sequence.length() - 5); //Roughly removing filling
        
        #define NUM_MISMATCHES_ALLOWED 2
        #define MIN_MATCHES 6
        uint32_t start_index = sequence.length() - probable_adapter.length();
        
        for (int i = start_index; i < sequence.size() - MIN_MATCHES - NUM_MISMATCHES_ALLOWED; ++i)
        {
            uint8_t mismatches {0};
            bool match = true;
            for (int j = 0; j < probable_adapter.length() && j + i < sequence.size() - MIN_MATCHES - NUM_MISMATCHES_ALLOWED; ++j)
            {
                if (sequence[i + j] != probable_adapter[j])
                {
                    ++mismatches;
                    if (mismatches > NUM_MISMATCHES_ALLOWED)
                    {
                        match = false;
                        break;
                    }
                }
            }
            if (match)
            {
                sequence = sequence.substr(0, i);
                adapterless_sequences.emplace_back(sequence);
                break;
            }
        }
    }
    fd_in.close();
    
    std::cout << "Removed adapter and have subset of size: " << adapterless_sequences.size() << std::endl;
    
    
    //STEP 1.3: With the subset of sequences, they shall represent the total set
    //Find the frequency distribution of the barcode
    #define BARCODE_LENGTH 8
    
    std::map<std::string, uint32_t> frequency_dist {};
    
    std::cout << "Creating barcode frequency distribution" <<std::endl;
    for (std::string s : adapterless_sequences)
    {
        std::string barcode = s.substr(s.length() - BARCODE_LENGTH);
        if (!frequency_dist.contains(barcode))
            frequency_dist.insert(std::make_pair(barcode, 1));
        else
            (*frequency_dist.find(barcode)).second++;
    }
    
    std::cout << "Writing barcode frequency distribution to file..." << std::endl;
    std:: ofstream fd_out;
    fd_out.open(write_dir + "task5_data_barcode_freq_dist.csv");
    if (!fd_out.is_open())
        exit(78);
    
    for (auto itr = frequency_dist.begin(); itr != frequency_dist.end();)
    {
        fd_out << (double)itr->second / adapterless_sequences.size();
        if (itr++ != frequency_dist.end())
            fd_out << ",";
    }
    fd_out.close();
    
    //Now that we have the frequency of the barcodes, we can use these to separate the set into samples.
    
    //Start by taking the most frequent barcode and create a sample of all the sequences that has that barcode
    //with a hamming distance of ≤2. (Possibly write the sample to file to have better runtime of memory).
    //Continue creating samples of the next most frequent barcode. Do this until the most frequent barcode
    //is less than a given amount set by analyzing the data (by eyes) times greater than the median barcode frequency.
    //OR until we have put ≥95%(?) of the sequence set into samples.
    
    std::cout << "Sampling sequences based on barcodes..." << std::endl;
    uint16_t num_sample {}; //There will not be more than >65.000 samples, but there will always be at least one
    std::vector<std::pair<std::string, uint32_t>> barcodes(frequency_dist.begin(), frequency_dist.end());
    std::sort(barcodes.begin(), barcodes.end(), [](auto a, auto b){ return a.second > b.second; });
    std::pair<std::string, uint32_t> barcode = barcodes.at(num_sample);
    //Since we are going to erase element whilst looping we want a non-continuous data structure, so we have
    //O(1) deletion time, and not O(n) as with a vector, hens the conversion to a list (linked-list).
    std::list<std::string> adapterless_sequences_list(adapterless_sequences.begin(), adapterless_sequences.end());
    adapterless_sequences.clear(); //Free the memory of storing all these values
    uint64_t total_num_of_sequences = adapterless_sequences_list.size();
    uint32_t median_barcode_frequency = barcodes.at(barcodes.size() / 2).second;
    std::vector<std::vector<std::string>> samples {};
    
    std::cout << "Barcode median frequency: " << median_barcode_frequency << std::endl;
    
    do
    {
        std::cout << "Sampling barcode: " << barcode.first << " (" << num_sample << "), " << barcode.second; // << std::endl;
        samples.emplace_back(std::vector<std::string>()); //Add new sample to samples
        for (auto itr = adapterless_sequences_list.begin(); itr != adapterless_sequences_list.end(); itr++)
        {
            //Expect that all sequences are long enough to be sub-stringed
            std::string bar = (*itr).substr((*itr).length() - BARCODE_LENGTH);
            if (hamming_distance(barcode.first, bar, 4) <= 3) //We found a match
            {
                std::string seq = (*itr).substr(0, (*itr).length() - BARCODE_LENGTH);
                samples.back().emplace_back(seq); //Add sequence to the sample
                itr = adapterless_sequences_list.erase(itr); //Remove the sequence from the total list and update the iterator to not destroy the loop
            }
        }
        std::cout << ", " << (double)samples.back().size() / total_num_of_sequences << std::endl;
        ++num_sample;
        if (barcodes.size() <= num_sample)
            break; //We have used all the barcodes, probably won't happen, but just to be sure
        barcode = barcodes.at(num_sample);
    } while ((double)barcode.second / total_num_of_sequences >= 0.02 && (double)adapterless_sequences.size() / total_num_of_sequences <= 0.05);
    uint32_t sum {};
    for (auto sam : samples)
        sum += sam.size();
    std::cout << "Percent of original set found to barcodes: " << (double)sum / total_num_of_sequences << std::endl;
    
    //TODO: Create a keyword tree for each sample; find length distribution of the sequences by DFS.
    //      Then write this distribution to file so it can be plotted.
    //      With the tree traverse it to a leaf node, following the most used path, this will
    //      give us the most frequent sequence in the sample.
    
    //for (std::vector<std::string> sample : samples)
    for (int i = 0; i < samples.size(); ++i)
    {
        std::vector<std::string> sample = samples.at(i);
        keyword_tree_t sample_tree(*"$");
        //Finding frequency distribution for sample
        std::vector<uint32_t> length_dist {};
        
        for (std::string seq : sample)
        {
            sample_tree.add_sequence(seq); //Building keyword tree at the same time
            uint32_t length = seq.length();
            while (length_dist.size() <= length)
                length_dist.emplace_back(0);
            ++length_dist.at(length);
        }
        /*std::vector<node_t *> stack {};
        stack.push_back(sample_tree.get_root()); //Important that this is not changed since the adapter was found
    
        std::cout << "Finding frequency distribution for sample " << i << ": " << barcodes.at(i).first << std::endl;
        while (!stack.empty())
        {
            node_t *current_node = stack.back();
            stack.pop_back();
            for (node_t *child : current_node->children)
                stack.push_back(child);
            if (current_node->is_leaf())
            {
                uint32_t length = sample_tree.get_label_length_to_root(current_node);
                //TODO: DEBUG HERE, why is there only one length
                while (length_dist.size() <= length)
                    length_dist.emplace_back(0);
                length_dist.at(length) += current_node->passes;
            }
        }*/
        
        fd_out.open(write_dir + "task5_data_freq_dist_sample_" + std::to_string(i) + ".csv");
        if (!fd_out.is_open())
            exit(78);
        std::cout << "Sample best sequence: " << sample_tree.get_most_frequant_sequence(2).substr(1) << std::endl;
        fd_out << sample_tree.get_most_frequant_sequence(2).substr(1) << ",";
        for (int i = 0; i < length_dist.size(); ++i)
        {
            fd_out << length_dist[i];
            if (i < length_dist.size() - 1)
                fd_out << ",";
        }
        fd_out.close();
    }
    
    
    //TODO: CONGRATULATIONS YOU ARE DONE, NOW PLOT YOUR DATA IN PYTHON, IF YOU HAVEN'T ALREADY, AND CELEBRATE
    
}


int main()
{
    std::string write_dir = "data/";
    std::string multiplexed_file = "data/MultiplexedSamples.txt";
    std::cout << "Starting task 5" << std::endl;
    
    gettimeofday(&t_start, NULL);
    
    task5(multiplexed_file,  0.5, write_dir);
    
    gettimeofday(&t_stop, NULL);
    t_total = WALLTIME(t_stop) - WALLTIME(t_start);
    printf("%.2lf seconds total runtime\n", t_total);
    
    std::cout << "Complete" << std::endl;
    
    return EXIT_SUCCESS;
}
