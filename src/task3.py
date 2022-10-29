
# Loop for s in S
#   add s to suffix tree
#   keep track of sequences through nodes
# Traverse suffix tree by going to child node with at least 95% of sequences visited
# if no option, stop, return current

# Other method for doing this
# Loop for s in S
#   add s to suffix tree
# Find greatest common substring
import csv
import math

import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

A = "TGGAATTCTCGGGTGCCAAGGAACTCCAGTCACACAGTGATCTCGTATGCCGTCTTCTGCTTG"

def seq_error_sequence(sequence, rates, error_dist):
    suffix_length = len(sequence)
    count = 0
    for i in range(suffix_length):
        if A[i] != sequence[i]:
            while len(error_dist) <= i:
                error_dist.append(0)
            error_dist[i] += 1
            count += 1

    rates.append(count / suffix_length)

def plot_estimate(x, y, func, func_inv, name, model):

    #plt.plot(x, y, 'o')
    plt.xlim(0, 100)
    plt.ylim(0, 100)
    plt.ylabel("%-of set matched sequences")
    plt.xlabel("k-% of |a| mismatches allowed")
    #plt.show()

    x[0] = 1.0
    #x = x[1:]
    #y = y[1:]
    #x_data = np.linspace(0, 1, num=40)

    #y_data = 3.45 * np.exp(1.334 * x) + np.random.normal(size=40)


    parameters, param_cov = curve_fit(func, x, y)

    fit_A = parameters[0]
    fit_B = parameters[1]
    print(fit_A)
    print(fit_B)

    SE = np.sqrt(np.diag(param_cov))
    SE_A = SE[0]
    SE_B = SE[1]


    print(F'The value of A is {fit_A:.5f} with standard error of {SE_A:.5f}.')
    print(F'The value of B is {fit_B:.5f} with standard error of {SE_B:.5f}.')

    rse = []
    for i in range(len(x)):
        rse.append((y[i] - func(x[i], *parameters)) ** 2)
    rmse = np.mean(rse)
    print("Mean Squared Error: ", rmse)

    n = int(func_inv(100, *parameters))

    interval = np.linspace(1, n, n)

    matches = []
    matches.append(func(interval[0], *parameters))
    interval = interval[1:]
    for i in interval:
        matches.append(func(i, *parameters))

    for i in range(len(matches) - 1, 0, -1):
        matches[i] -= matches[i - 1]
        matches[i] *= i / 100
    matches[0] *= 1 / 100
    rate = sum(matches) / n
    rate *= 100

    #ss_res = np.dot((y - func(x, *parameters)), (y - func(x, *parameters)))
    #ymean = np.mean(y)
    #ss_tot = np.dot((y - ymean), (y - ymean))
    #print("Mean R :", 1 - ss_res / ss_tot)

    plt.title(f"Estimate of {name} error distribution in dataset\nRMSE: {rmse:.0f}, rate: {rate:.1f}%")

    t_1 = np.linspace(1, 100, 100)

    fit_y = func(t_1, *parameters)
    plt.plot(x, y, 'o', label='data points')
    plt.plot(t_1, fit_y, '-', label=f'{model}')
    plt.legend()

    plt.savefig(f"../plots/task3_estimate_{name}")
    plt.close()

def plot_distribution(dist, percent):
    plt.plot(dist)
    k = math.floor(len(A) * percent)  # Number of mismatches allowed
    plt.axvline(x=k + 3, color='tab:orange')
    plt.axvline(x=k + 10, color='tab:green')
    #plt.legend(loc="upper left")
    plt.ylabel("Number of missmatches")
    plt.xlabel("Index in adapter/primer")
    plt.title(f"Distribution of nucleotide errors in\nsequences suffix with prefix of adapter for {(percent * 100):.0f}%")
    plt.legend(["Sequencing errors", "k + 3", "k + 10"], loc="upper right")
    plt.savefig(f"../plots/task3_distribution_errors_{(percent * 100):.0f}")
    plt.close()

if __name__ == '__main__':

    # TODO: Make this dynamically fetched from data files
    # Plot estimate for matches for k% error
    match_percent = [19.2, 55.2, 85.2, 78.0, 85.4]
    error_percent = [0.0, 10.0, 25.0, 10.0, 25.0]

    def fit_log(x, a, b, c):
        return a + b * np.log(x * c)
    def fit_log_inv(y, a, b, c):
        return np.exp((y - a) / b) / c
    def fit_linear(x, a, b):
        return a + b * x
    def fit_linear_inv(y, a, b):
        return (y - a) / b

    plot_estimate(error_percent, match_percent, fit_log, fit_log_inv, "log", "a + b * ln(x * c) = y")
    plot_estimate(error_percent, match_percent, fit_linear, fit_linear_inv, "linear", "a + bx = y")


    print("Starting Task 3...")
    print("Reading file...")
    data = []


    #with open("../data/task2_data_10_suffix.csv", "r") as f:
    with open("../data/task2_data_10_suffix.csv", newline="") as csv_file:
        csv_reader = csv.reader(csv_file, quoting=csv.QUOTE_NONNUMERIC)
        data = list(csv_reader)
    print("Read file")

    #data => {sequence_num, suffix_start}
    case10 = data  # indices of suffix for each sequence with 10% error rate

    with open("../data/task2_data_25_suffix.csv", newline="") as csv_file:
        csv_reader = csv.reader(csv_file, quoting=csv.QUOTE_NONNUMERIC)
        data = list(csv_reader)
    print("Read file")
    case25 = data  # indices of suffix for each sequence with 25% error rate

    sequences = []
    with open("../data/s_3_sequence_1M.txt", "r") as file:
        sequences = file.read().splitlines()


    rates = []
    error_dist = []
    offset = 0
    print("Processing data...")
    for i in range(len(sequences)):
        if i % 100000 == 0:
            print(i)
        if int(case10[offset][0]) != i:
            continue
        suffix = int(case10[offset][1])
        offset += 1
        sequence = sequences[i][suffix:]

        seq_error_sequence(sequence, rates, error_dist)

        if (offset >= len(case10)):
            break

    plot_distribution(error_dist, 0.1)
    rate = sum(rates) / len(rates)
    print(f"Rate 10%-mismatch: {rate}")
    print("Data processed for 10%-mismatch")

    rates = []
    error_dist = []
    offset = 0
    print("Processing data...")
    for i in range(len(sequences)):
        if i % 100000 == 0:
            print(i)
        if int(case25[offset][0]) != i:
            continue
        suffix = int(case25[offset][1])
        offset += 1
        sequence = sequences[i][suffix:]

        seq_error_sequence(sequence, rates, error_dist)

        if (offset >= len(case25)):
            break

    plot_distribution(error_dist, 0.25)
    rate = sum(rates) / len(rates)
    print(f"Rate 25%-mismatch: {rate}")
    print("Data processed for 25%-mismatch")