#include <iostream>
#include <map>
#include "../task4/keyword_tree.cpp"

void task5(std::string filepath, double certainty, std::string write_dir)
{
    //STEP 1.1: Find the adapter
    keyword_tree_t tree(*"$");
    std::ifstream fd_in;
    fd_in.open(filepath);
    if (!fd_in.is_open())
        exit(55);
    
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
    #define BARCODE_LENGTH 6
    
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
        fd_out << itr->second;
        if (itr++ != frequency_dist.end())
            fd_out << ",";
    }
    fd_out.close();
    
    //TODO: Now that we have the frequency of the barcodes, we can use these to separate the set into samples.
    //TODO: Start by taking the most frequent barcode and create a sample of all the sequences that has that barcode
    //      with a hamming distance of ≤2. (Possibly write the sample to file to have better runtime of memory).
    //      Continue creating samples of the next most frequent barcode. Do this until the most frequent barcode
    //      is within 10% of the median barcode frequency. OR until we have put ≥95%(?) of the sequence set into samples.
    
    //TODO: Once the de-multiplexing is done and we have our samples, we can count the number os sequences in the samples.
    //      Then write data to be plotted as [barcode, number of sequences with barcode].
    
    
    //TODO: While counting the number of sequences in the sample we can also note the length distribution of the
    //      sequences in each sample. Then write this distribution to file so it can be plotted.
    
    //TODO: Also while processing the sample, put the sequences into a keyword tree (as done earlier, above).
    //      With the whole tree created traverse it to a leaf node, following the most used path, this will
    //      give us the most frequent sequence in the sample.
    
    
    //TODO: CONGRATULATIONS YOU ARE DONE, NOW PLOT YOUR DATA IN PYTHON, IF YOU HAVEN'T ALREADY AND CELEBRATE
    
}


int main()
{
    std::string write_dir = "../../../data/";
    std::string multiplexed_file = "../../../data/MultiplexedSamples.txt";
    std::cout << "Starting task 4" << std::endl;
    task5(multiplexed_file,  0.5, write_dir);
    std::cout << "Complete" << std::endl;
    return EXIT_SUCCESS;
}
