

# Without indels
# k = x% of |a| (mismatches)
# misses = 0
# Loop for s in S
#   sequence_done = false
#   Loop for j = 0 to |s| - 1 //Check sequence
#       if sequence_done
#           break
#       Loop for i = 0 to |a| - 1 //Check diagonal
#           if j + i >= |s|
#               sequence_done = true
#               add to list j //number of letters in s without adapter
#               break
#           if s[j + i] == a[i] continue
#           else
#               misses++
#               if misses > k
#                   break
#
#

# With indels
# k = x% of |a| (mismatches)
# for s in S
#   table = ED(a, s, k) //optimize by red square theorem
#   for i = |a| to 0 //Loop through last row
#       if table[|s|, i] <= k
#           add to list backtrack_find_first_col_ret_row_num(|s|, i) - 1
#           break
#
import math
import matplotlib.pyplot as plt

def task2(a, percent_error, filepath):
    # Using readline()
    file = open(filepath, 'r')
    count = 0
    length_of_sequences = []
    length_of_sequences_edit = []
    k = math.floor(len(a) * percent_error)  # Number of mismatches allowed
    while True:
        count += 1
        if count % 10000 == 0:
            print(count)
        # Get next line from file
        sequence = file.readline()

        # if line is empty
        # end of file is reached
        if not sequence:
            break

        sequence_done = False
        for j in range(len(sequence) - 1):  # For each letter in the sequence
            if sequence_done:
                break
            misses = 0
            for i in range(len(a) - 1):  # For each letter in primer a
                if j + i >= len(sequence):
                    sequence_done = True
                    while len(length_of_sequences) <= j:
                        length_of_sequences.append(0)
                    length_of_sequences[j] += 1
                    match_length = i
                    if match_length > misses + 3:
                        while len(length_of_sequences_edit) <= j:
                            length_of_sequences_edit.append(0)
                        length_of_sequences_edit[j] += 1
                    break
                if sequence[j + i] == a[i]:
                    continue
                else:
                    misses += 1
                    if misses > k:
                        break

    file.close()

    number_of_sequences = count - 1
    length_of_matching_sequences = length_of_sequences

    number_of_matches = sum(length_of_matching_sequences)
    percent = "{:2.1f}".format(100 * number_of_matches / number_of_sequences)
    label = str("Total number of matches with " + str(int(k)) + " mismatches: " + str(int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend(loc="upper left")
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    title = str("Length distribution of sequences without " + str(int(k)) + " mismatch prefix-suffix-matching adapter")
    plt.title(title)
    name = "../plots/task2_distribution_" + str(percent_error * 100).split(".")[0]
    plt.savefig(name)


    length_of_matching_sequences = length_of_sequences_edit[1:]
    number_of_matches = sum(length_of_matching_sequences)
    percent = "{:2.1f}".format(100 * number_of_matches / number_of_sequences)
    label = str("Total number of matches with " + str(int(k)) + " mismatches: " + str(int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend(loc="upper left")
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    title = str("Length distribution of sequences without " + str(int(k)) + " mismatch prefix-suffix-matching adapter edit")
    plt.title(title)
    name = "../plots/task2_distribution_edit_" + str(percent_error * 100).split(".")[0]
    plt.savefig(name)


if __name__ == '__main__':
    a = "TGGAATTCTCGGGTGCCAAGGAACTCCAGTCACACAGTGATCTCGTATGCCGTCTTCTGCTTG"
    task2(a, 0.1, "../data/s_3_sequence_1M.txt")
    task2(a, 0.25, "../data/s_3_sequence_1M.txt")

    print("Done")