#!/usr/bin/python

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
import sys
import threading

import matplotlib.pyplot as plt
import numpy as np


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
    label = str("Total number of matches with " + str(int(k)) + " mismatches: " + str(
        int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend(loc="upper left")
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    title = str("Length distribution of sequences without " + str(int(k)) + " mismatch prefix-suffix-matching adapter")
    plt.title(title)
    name = "plots/task2_distribution_" + str(percent_error * 100).split(".")[0]
    plt.savefig(name)

    plt.close()

    length_of_matching_sequences = length_of_sequences_edit[1:]
    number_of_matches = sum(length_of_matching_sequences)
    percent = "{:2.1f}".format(100 * number_of_matches / number_of_sequences)
    label = str("Total number of matches with " + str(int(k)) + " mismatches: " + str(
        int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend(loc="upper left")
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    title = str(
        "Length distribution of sequences without " + str(int(k)) + " mismatch prefix-suffix-matching adapter edit")
    plt.title(title)
    name = "plots/task2_distribution_edit_" + str(percent_error * 100).split(".")[0]
    plt.savefig(name)
    plt.close()


def score_ED(s, r):
    if s != r:
        return 1
    return 0


def ED(S, R, k):
    rows = len(R) + 1
    cols = len(S) + 1
    table = np.zeros(shape=(rows, cols), dtype=int)
    inf = sys.maxsize / 2
    start_bt_col = cols - 1
    for col in range(2, cols):
        table[0, col] = inf

    for col in range(1, cols):
        if col + k + 1 >= cols:
            break
        if col >= rows:
            start_bt_col = col + k - 1
            break
        table[col, col + k + 1] = inf
    # Fill the table

    start_row = 1
    if cols < rows:
        start_row = max(1, rows - (cols - 1) - k)
    if start_row > 1:
        for col in range(1, cols):
            table[start_row - 1, col] = inf

    still_valid = False
    inner_col = 1
    for row in range(start_row, rows):
        for col in range(inner_col, min(cols, row + k + 1)):
            table[row, col] = min(table[row - 1, col] + 1,
                                  min(table[row, col - 1] + 1,
                                      table[row - 1, col - 1] + score_ED(S[col - 1], R[row - 1])))
            # if table[row, col] <= k:
            #   still_valid = True

        for inner_row in range(row + 1, rows):
            table[inner_row, inner_col] = min(table[inner_row - 1, inner_col] + 1,
                                              min(table[inner_row, inner_col - 1] + 1,
                                                  table[inner_row - 1, inner_col - 1] + score_ED(S[inner_col - 1],
                                                                                                 R[inner_row - 1])))
            # if table[inner_row, inner_col] <= k:
            #   still_valid = True

        for col in range(inner_col, min(cols, row + k + 1)):
            if table[row, col] <= k:
                still_valid = True
                break
        for inner_row in range(row + 1, rows):
            if table[inner_row, inner_col] <= k:
                still_valid = True
                break
        if not still_valid:
            start_bt_col = inner_col
            break
        inner_col += 1
        still_valid = False
    # Save format
    # tableData = np.zeros(shape=(len(R) + 2, len(S) + 2), dtype=np.dtype('U100'))
    #
    # tableData[0, 0] = "+"
    # tableData[0, 1] = tableData[1, 0] = "¢"
    # for row in range(0, len(R) + 2):
    #    for col in range(0, len(S) + 2):
    #        if row > 0 and col > 0:
    #            tableData[row, col] = "%d" % table[row - 1, col - 1]
    #
    #        elif row == 0 and col > 1:
    #            tableData[row, col] = S[col - 2]
    #        elif col == 0 and row > 1:
    #            tableData[row, col] = R[row - 2]

    # print_latex_table(tableData, len(R) + 2, len(S) + 2)

    return table, start_bt_col


def back_track(matrix, row, col, S, R):
    hits = 0

    while True:
        # It came from the diagonal
        hit = score_ED(S[col - 1], R[row - 1])
        if matrix[row, col] == matrix[row - 1, col - 1] + hit:
            if hit == 0:
                hits += 1
            row -= 1
            col -= 1
        elif matrix[row, col] == matrix[row, col - 1] + 1:
            col -= 1
        elif matrix[row, col] == matrix[row - 1, col] + 1:
            row -= 1
        else:
            exit(4)  # Back track not valid
        if col <= 1:
            if col < 1:
                exit(3)  # This should not happen, should have quit on row 1
            break
        if row < 1:
            exit(2)  # This should not happen, we need to end up in first row

    suffix = row - 1
    return hits, suffix


def indel_do_sequence(primer, sequences, k, list, lock):
    length_list = {}
    # for sequence in sequences:
    while len(sequences) > 0:
        sequence = sequences.pop()
        print(len(sequences))
        matrix, start_bt_col = ED(primer, sequence, k)
        # matrix = ED("GGC", "CTCTAGC", 1)
        for col in range(start_bt_col, 2, -1):
            row = matrix.shape[0] - 1
            if matrix[row, col] > k:
                continue
            hits, suffix = back_track(matrix, row, col, primer, sequence)
            if hits > 3:
                if suffix in length_list:
                    length_list[suffix] += 1
                else:
                    length_list[suffix] = 1
                s = sum(length_list)
                if s % 100 == 0:
                    print(len(sequences))
                break
    print("Done, writing to summary")
    lock.acquire()
    for i in range(len(length_list)):
        while len(list) <= i:
            list.append(0)
        length_list[suffix] += length_list[i]
    lock.release()


def split(list, n):
    k, m = divmod(len(list), n)
    return [list[i * k + min(i, m):(i + 1) * k + min(i + 1, m)] for i in range(n)]


import csv


def task2_indel(a, percent_error, filepath):
    with open(filepath, newline='') as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
        data = list(reader)

    f.close()
    print(data)

    number_of_sequences = data[0][-1]
    length_of_matching_sequences = data[0][0:-1]

    number_of_matches = sum(length_of_matching_sequences)
    print("Number of perfect prefix-suffix-matches: ",
          number_of_matches)  # Since we increment index matching length the sum of the list will be equal to the number of matches
    print("Number of sequences in set: ", number_of_sequences)
    percent = "{:2.1f}".format(100 * number_of_matches / number_of_sequences)
    label = str("Total number of perfecrt matches: " + str(int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend(loc="upper left")
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    plt.title("Length distribution of sequences without perfect prefix-suffix-matching adapter")

    name = "plots/task2_distribution_indels" + str(percent_error * 100).split(".")[0]
    plt.savefig(name)

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

    name = "plots/task2_distribution_edit_indels" + str(percent_error * 100).split(".")[0]
    plt.savefig(name)
    plt.close()

    return
    number_of_sequences = count

    length_of_matching_sequences = length_of_sequences_edit[1:]
    number_of_matches = sum(length_of_matching_sequences)
    percent = "{:2.1f}".format(100 * number_of_matches / number_of_sequences)
    label = str("Total number of matches with " + str(int(k)) + " mismatches: " + str(
        int(number_of_matches)) + ": " + percent + "% of set")
    plt.plot(length_of_matching_sequences, label=label)
    plt.legend(loc="upper left")
    plt.ylabel("Occurrences")
    plt.xlabel("Length of sequence without adapter")
    title = str(
        "Length distribution of sequences with " + str(
            int(k)) + " mismatch\n prefix-suffix-matching adapter edit with indels")
    plt.title(title)
    name = "plots/task2_distribution_edit_indels" + str(percent_error * 100).split(".")[0]
    plt.savefig(name)
    plt.close()


if __name__ == '__main__':
    a = "TGGAATTCTCGGGTGCCAAGGAACTCCAGTCACACAGTGATCTCGTATGCCGTCTTCTGCTTG"
    task2(a, 0.1, "data/s_3_sequence_1M.txt")
    task2(a, 0.25, "data/s_3_sequence_1M.txt")
    print("Starting")
    task2_indel(a, 0.1, "data/task2_data_10.csv")
    task2_indel(a, 0.25, "data/task2_data_25.csv")

    print("Done")
