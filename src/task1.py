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
import csv


def task1(filename):

    with open(filename, newline='') as f:
        reader = csv.reader(f, quoting = csv.QUOTE_NONNUMERIC)
        data = list(reader)

    #print(data)

    file = open(filename, "r")  # Open the data file
    number_of_sequences = data[0][-1]
    length_of_matching_sequences = data[0][0:-1]

    number_of_matches = sum(length_of_matching_sequences)
    print("Number of perfect prefix-suffix-matches: ",
          number_of_matches)  # Since we increment index matching length the sum of the list will be equal to the number of matches
    print("Number of sequences in set: ", number_of_sequences)
    percent = "{:2.1f}".format(100 * number_of_matches / number_of_sequences)
    label = str("Total number of perfecrt matches: " + str(int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend()
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    plt.title("Length distribution of sequences without perfect prefix-suffix-matching adapter")

    plt.savefig("plots/task1_distribution")

    plt.close()

    number_of_sequences = data[0][-1]
    length_of_matching_sequences = data[0][1:-3]
    number_of_matches = sum(length_of_matching_sequences)
    percent = "{:2.1f}".format(100 * number_of_matches / number_of_sequences)
    label = str("Total number of perfecrt matches: " + str(int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend(loc="upper left")
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    plt.title("Length distribution of sequences without perfect prefix-suffix-matching adapter")

    plt.savefig("plots/task1_distribution_edit")



if __name__ == '__main__':
    task1("data/task1_data.csv")
    print("Done")