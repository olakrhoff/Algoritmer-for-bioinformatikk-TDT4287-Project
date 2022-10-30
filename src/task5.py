

import csv
import matplotlib.pyplot as plt

def task5(filepath, plot_dir):
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



if __name__ == '__main__':
    print("Starting task 5 plotting")
    data_dir = "../data/"
    plot_dir = "../plots/"
    task5(data_dir + "task5_data_barcode_freq_dist.csv", plot_dir)
    print("Done")