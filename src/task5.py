

import csv
import os

import matplotlib.pyplot as plt
import numpy as np


def task5(filepath, samples, plot_dir):
    with open(filepath, newline='') as f:
        reader = csv.reader(f, quoting=csv.QUOTE_NONNUMERIC)
        data = list(reader)

    frequency_of_barcodes = data[0][:-2]

    plt.plot(frequency_of_barcodes)
    plt.legend()
    plt.ylabel("Frequency")
    plt.xlabel("Barcodes")
    plt.title("Frequency distribution of barcodes")

    plt.savefig(f"{plot_dir}/task5_barcode_frequency_distribution")

    plt.close()

    while True:
        with open(samples, newline="") as f:
            reader = csv.reader(f)
            data = list(reader)

        samples0 = samples.split(".csv")[0]
        num = int(samples0[-1])
        samples = samples0[:-1] + str(num + 1) + ".csv"

        length_dist_sample = data[0][1:-2]

        plt.bar(np.linspace(0, len(length_dist_sample), len(length_dist_sample)), length_dist_sample)
        plt.legend([f"Most common sequence:\n {data[0][0]} ({len(data[0][0])})"], loc="upper left")
        plt.ylabel("Occurrences")
        plt.xlabel(f"Length of sequence")
        plt.title(f"Length distribution of sequences in sample {num}")

        plt.savefig(f"{plot_dir}/task5_sample_{num}_length_distribution")

        plt.close()



        if not os.path.isfile(samples):
            break  # No more samples


if __name__ == '__main__':
    print("Starting task 5 plotting")
    data_dir = "data/"
    plot_dir = "plots/"
    task5(data_dir + "task5_data_barcode_freq_dist.csv", data_dir + "task5_data_freq_dist_sample_0.csv", plot_dir)
    print("Done")