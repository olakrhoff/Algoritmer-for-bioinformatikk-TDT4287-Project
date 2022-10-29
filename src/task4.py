




import matplotlib.pyplot as plt
import csv

def task4(filename_len_dist, filename_freq_dist, file):

    # Plot length distribution for s - |a|
    with open(filename_len_dist, newline='') as f:
        reader = csv.reader(f, quoting = csv.QUOTE_NONNUMERIC)
        data = list(reader)

    length_of_matching_sequences = data[0]

    plt.plot(length_of_matching_sequences)
    plt.legend()
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    plt.title("Length distribution of sequences without adapter")

    plt.savefig(f"plots/task4_length_distribution_{file}")

    plt.close()

    # Plot length distribution for s - |a|
    with open(filename_freq_dist, newline='') as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
        data = list(reader)

    frequency_of_unique_sequences = data[0][:-2]

    plt.plot(frequency_of_unique_sequences)
    plt.legend()
    plt.ylabel("Frequency")
    plt.xlabel("Unique sequences")
    plt.title("Frequency distribution of unique sequences")

    plt.savefig(f"plots/task4_unique_frequency_distribution_{file}")

    plt.close()




if __name__ == '__main__':
    print("Starting task 4")
    task4("data/task4_data_len_dist_tdt.csv", "data/task4_data_unique_freq_dist_tdt.csv", "tdt")
    task4("data/task4_data_len_dist_s_3.csv", "data/task4_data_unique_freq_dist_s_3.csv", "s_3")
    task4("data/task4_data_len_dist_seq.csv", "data/task4_data_unique_freq_dist_seq.csv", "seq")
    print("Done")