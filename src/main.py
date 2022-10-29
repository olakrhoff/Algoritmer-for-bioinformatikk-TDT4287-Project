import string

import numpy as np


def print_latex_table(T, rows, cols):
    spacing = ""
    for col in range(cols):
        spacing += "| m{1em} "
    spacing += "|"

    data = ""
    for row in range(rows):
        data += "           \\hline\n        "
        for col in range(cols - 1):
            data += T[row, col] + " & "
        data += T[row, cols - 1]
        data += "\\\\\n"
    data += "           \\hline\n"

    latex = "\\begin{table}[H]\n" \
            "   \\begin{center}\n" \
            "       \\begin{tabular}{" + spacing + " }\n" \
            + data + \
            "       \\end{tabular}\n" \
            "   \\end{center}\n" \
            "   \\caption{Caption here}\n" \
            "\\end{table}\n"

    print(latex)


def f(S, R):
    if S == R:
        return 1
    return 0


def lcs(S, R):
    table = np.zeros(shape=(len(R) + 1, len(S) + 1), dtype=int)

    # Already correctly init

    # Fill the table
    for row in range(1, len(R) + 1):
        for col in range(1, len(S) + 1):
            table[row, col] = max(table[row - 1, col],
                                  max(table[row, col - 1], table[row - 1, col - 1] + f(R[row - 1], S[col - 1])))

    # Save format
    tableData = np.zeros(shape=(len(R) + 2, len(S) + 2), dtype=str)
    tableData[0, 0] = "+"
    tableData[0, 1] = tableData[1, 0] = "¢"
    for row in range(0, len(R) + 2):
        for col in range(0, len(S) + 2):
            if row > 0 and col > 0:
                tableData[row, col] = str(table[row - 1, col - 1])

            elif row == 0 and col > 1:
                tableData[row, col] = S[col - 2]
            elif col == 0 and row > 1:
                tableData[row, col] = R[row - 2]

    print_latex_table(tableData, len(R) + 2, len(S) + 2)
    return table


def lcs_r(rows, cols):
    table = np.zeros(shape=(rows, cols), dtype=int)

    for col in range(cols):
        table[0, col] = col + 1
    for row in range(rows):
        table[row, 0] = row + 1

    # Fill the table
    for row in range(1, rows):
        for col in range(1, cols):
            table[row, col] = table[row - 1, col] + table[row, col - 1] + table[row - 1, col - 1] + 1
    return table


def lcs_rec(S: str, R: str):
    if len(S) == 0 or len(R) == 0:
        return 0

    return max(lcs_rec(S[0:-1], R), max(lcs_rec(S, R[0:-1]), lcs_rec(S[0:-1], R[0:-1]) + f(R[-1], S[-1])))


def score_ED(s, r):
    if s != r:
        return 1
    return 0


def ED(S, R):
    table = np.zeros(shape=(len(R) + 1, len(S) + 1), dtype=int)

    for col in range(len(S) + 1):
        table[0, col] = col
    for row in range(len(R) + 1):
        table[row, 0] = row

    # Fill the table
    for row in range(1, len(R) + 1):
        for col in range(1, len(S) + 1):
            table[row, col] = min(table[row - 1, col] + 1,
                                  min(table[row, col - 1] + 1,
                                      table[row - 1, col - 1] + score_ED(S[col - 1], R[row - 1])))

    # Save format
    tableData = np.zeros(shape=(len(R) + 2, len(S) + 2), dtype=np.dtype('U100'))

    tableData[0, 0] = "+"
    tableData[0, 1] = tableData[1, 0] = "¢"
    for row in range(0, len(R) + 2):
        for col in range(0, len(S) + 2):
            if row > 0 and col > 0:
                tableData[row, col] = "%d" % table[row - 1, col - 1]

            elif row == 0 and col > 1:
                tableData[row, col] = S[col - 2]
            elif col == 0 and row > 1:
                tableData[row, col] = R[row - 2]

    print_latex_table(tableData, len(R) + 2, len(S) + 2)

    return table


def score_GA(s, r):
    if s == '' or r == '':
        return -2  # indel
    if s == r:
        return 1  # match
    return -2  # mismatch


def GA(S, R):
    table = np.zeros(shape=(len(R) + 1, len(S) + 1), dtype=int)

    for col in range(len(S) + 1):
        table[0, col] = -col
    for row in range(len(R) + 1):
        table[row, 0] = -row

    # Fill the table
    for row in range(1, len(R) + 1):
        for col in range(1, len(S) + 1):
            table[row, col] = max(table[row - 1, col] + score_GA(S[col - 1], ''),
                                  max(table[row, col - 1] + score_GA('', R[row - 1]),
                                      table[row - 1, col - 1] + score_GA(S[col - 1], R[row - 1])))

    # Save format
    tableData = np.zeros(shape=(len(R) + 2, len(S) + 2), dtype=np.dtype('U100'))

    tableData[0, 0] = "+"
    tableData[0, 1] = tableData[1, 0] = "¢"
    for row in range(0, len(R) + 2):
        for col in range(0, len(S) + 2):
            if row > 0 and col > 0:
                tableData[row, col] = "%d" % table[row - 1, col - 1]

            elif row == 0 and col > 1:
                tableData[row, col] = S[col - 2]
            elif col == 0 and row > 1:
                tableData[row, col] = R[row - 2]

    print_latex_table(tableData, len(R) + 2, len(S) + 2)
    return table


def LA(S, R):
    table = np.zeros(shape=(len(R) + 1, len(S) + 1), dtype=int)

    # Fill the table
    for row in range(1, len(R) + 1):
        for col in range(1, len(S) + 1):
            table[row, col] = max(table[row - 1, col] + score_GA(S[col - 1], ''),
                                  max(table[row, col - 1] + score_GA('', R[row - 1]),
                                      max(table[row - 1, col - 1] + score_GA(S[col - 1], R[row - 1]),
                                          0)))
    # Save format
    tableData = np.zeros(shape=(len(R) + 2, len(S) + 2), dtype=np.dtype('U100'))

    tableData[0, 0] = "+"
    tableData[0, 1] = tableData[1, 0] = "¢"
    for row in range(0, len(R) + 2):
        for col in range(0, len(S) + 2):
            if row > 0 and col > 0:
                tableData[row, col] = "%d" % table[row - 1, col - 1]

            elif row == 0 and col > 1:
                tableData[row, col] = S[col - 2]
            elif col == 0 and row > 1:
                tableData[row, col] = R[row - 2]

    print_latex_table(tableData, len(R) + 2, len(S) + 2)
    return table


def GA_A(S, R):
    def score_GA_A(s, r):
        if s == r:
            return 1  # match
        return -2  # mismatch

    sigma = -1
    rho = -3

    rows = len(R) + 1
    cols = len(S) + 1

    GA = np.zeros(shape=(rows, cols), dtype=int)

    for col in range(cols):
        GA[0, col] = -col
    for row in range(rows):
        GA[row, 0] = -row

    GA_I = GA.copy()
    GA_D = GA.copy()

    # Fill the tables
    # Fill out row in GA_I
    for col in range(1, cols):
        GA_I[1, col] = max(GA_I[0, col] + sigma,
                           GA[0, col] + (sigma + rho))
    # Fill out col in GA_D
    for row in range(1, rows):
        GA_D[row, 1] = max(GA_D[row, 0] + sigma,
                           GA[row, 0] + (sigma + rho))

    for row in range(1, rows):
        for col in range(1, cols):

            GA[row, col] = max(GA[row - 1, col - 1] + score_GA_A(S[col - 1], R[row - 1]),
                               max(GA_I[row, col],
                                   GA_D[row, col]))
            if col != cols - 1:
                GA_D[row, col + 1] = max(GA_D[row, col] + sigma,
                                         GA[row, col] + (sigma + rho))
            if row != rows - 1:
                GA_I[row + 1, col] = max(GA_I[row, col] + sigma,
                                         GA[row, col] + (sigma + rho))

    # Save format
    dataGA = np.zeros(shape=(rows + 1, cols + 1), dtype=np.dtype('U100'))
    dataGA_I = dataGA.copy()
    dataGA_D = dataGA.copy()

    data = [(dataGA, GA), (dataGA_I, GA_I), (dataGA_D, GA_D)]

    for (tableData, table) in data:
        tableData[0, 0] = "+"
        tableData[0, 1] = tableData[1, 0] = "¢"
        for row in range(0, rows + 1):
            for col in range(0, cols + 1):
                if row > 0 and col > 0:
                    tableData[row, col] = "%d" % table[row - 1, col - 1]

                elif row == 0 and col > 1:
                    tableData[row, col] = S[col - 2]
                elif col == 0 and row > 1:
                    tableData[row, col] = R[row - 2]

        print_latex_table(tableData, len(R) + 2, len(S) + 2)
    return GA, GA_I, GA_D



def GA_k_diff(S, R, k):
    table = np.zeros(shape=(len(R) + 1, len(S) + 1), dtype=int)

    for row in range(0, len(R) + 1):
        for col in range(0, len(S) + 1):
            table[row, col] = -100

    for col in range(len(S) + 1):
        table[0, col] = -col
    for row in range(len(R) + 1):
        table[row, 0] = -row

    score = lambda s, r: -1 if (s != r) else 0
    # Fill the table
    for row in range(1, len(R) + 1):
        for col in range(max(row - k, 1), min(row + k + 1, len(S) + 1)):
            table[row, col] = max(table[row - 1, col] + -1,
                                  max(table[row, col - 1] + -1,
                                      table[row - 1, col - 1] + score(S[col - 1], R[row - 1])))

    # Save format
    tableData = np.zeros(shape=(len(R) + 2, len(S) + 2), dtype=np.dtype('U100'))

    tableData[0, 0] = "+"
    tableData[0, 1] = tableData[1, 0] = "¢"
    for row in range(0, len(R) + 2):
        for col in range(0, len(S) + 2):
            if row > 0 and col > 0:
                tableData[row, col] = "%d" % table[row - 1, col - 1]

            elif row == 0 and col > 1:
                tableData[row, col] = S[col - 2]
            elif col == 0 and row > 1:
                tableData[row, col] = R[row - 2]

    print_latex_table(tableData, len(R) + 2, len(S) + 2)
    return table


if __name__ == '__main__':
    S = "GGCTCTA"
    R = "CTCTAGC"

    # print(lcs(S, R))
    N = 7
    # print(lcs_r(N, N))
    # print(lcs_rec(S, R))

    # print(ED(S, R))
    # print(GA(S, R))
    # print(LA(S, R))

    # print(GA_A(S, R)[0])

    k = 2
    #print(GA_k_diff(S, R, k))

