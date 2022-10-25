# Create implicit suffix tree of a (adapter) IS_a
# Loop through all s in S
# Copy IS_a -> IS_ac
# Add s to IS_ac
# Make IS_ac explicit -> ES_ac (note suffix in inner nodes, if leaf node only has $, add to list in parent)
# if parent has prefix leaf node, add solution to list (length_of_sequence(suffix_start)) break loop
# else continue
# goto loop

# Print length of list -> number of sequences found
# Plot dist. of values in list
import matplotlib.pyplot as plt

from Utilities.SuffixTree import SuffixTree


def task1(primer_a, filename):
    file = open(filename, "r")  # Open the data file

    # Create implicit suffix tree
    suffixTree = SuffixTree()

    suffixTree.add_sequence(primer_a + "$")

    length_of_matching_sequences = []

    while True:
        sequence = file.readline()
        if not sequence:
            break  # Reached the end of the file
        suffix_start = suffixTree.prefix_suffix_match(sequence)
        while len(length_of_matching_sequences) < suffix_start:
            length_of_matching_sequences.append(0)  # Expand the list so we can increment the correct index
        length_of_matching_sequences[suffix_start] += 1  # Suffix start is equal to the length of the remaining sequence

    print("Number of perfect prefix-suffix-matches: ",
          sum(length_of_matching_sequences))  # Since we increment index matching length the sum of the list will be equal to the number of matches

    plt.plot(length_of_matching_sequences)
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    plt.title("Length distribution of sequences without perfect prefix-suffix-matching adapter")

    plt.show()


if __name__ == '__main__':
    a = "TGGAATTCTCGGGTGCCAAGGAACTCCAGTCACACAGTGATCTCGTATGCCGTCTTCTGCTTG"
    #task1(a, "../data/s_3_sequence_1M.txt")
    suffixTree = SuffixTree("rrerirra")
    suffixTree.build()