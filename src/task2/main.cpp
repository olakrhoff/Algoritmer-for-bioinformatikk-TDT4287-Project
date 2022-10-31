#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <set>

#include <sys/time.h>
#define WALLTIME(t) ((double)(t).tv_sec + 1e-6 * (double)(t).tv_usec)

struct timeval
        t_start,
        t_stop;
double
t_total;

template<typename T>
struct matrix_t
{
    uint32_t rows {0}, cols {0};
    T *data {nullptr};
    
    matrix_t() = default;
    
    matrix_t(uint32_t rows, uint32_t cols) : rows(rows), cols(cols)
    {
        data = new T[rows * cols]();
    }
    
    virtual ~matrix_t()
    {
        delete[] data;
    }
    
    inline uint32_t get_infinity() const
    {
        return UINT32_MAX / 2;
    }
    
    inline void set(uint32_t row, uint32_t col, T value)
    {
        data[row * cols + col] = value;
    }
    
    inline T at(uint32_t row, uint32_t col) const
    {
        return data[row * cols + col];
    }
    
    friend std::ostream &operator<<(std::ostream &os, const matrix_t *matrix)
    {
        for (uint32_t row = 0; row < matrix->rows; ++row)
        {
            for (uint32_t col = 0; col < matrix->cols; ++col)
            {
                os << matrix->at(row, col) << " ";
            }
            os << "\n";
        }
        
        return os;
    }
};

typedef struct ED_data
{
    ED_data(matrix_t<uint32_t> *matrix, uint64_t startBtCol) : matrix(matrix), start_bt_col(startBtCol)
    {}
    
    matrix_t<uint32_t> *matrix {nullptr};
    uint64_t start_bt_col {0};
    
    virtual ~ED_data()
    {
        delete matrix;
    }
} ED_data_t;

typedef struct BT_data
{
    uint64_t hits {0}, suffix {0};
} BT_data_t;

template<typename T>
T min(T a, T b)
{
    if (a < b)
        return a;
    return b;
}

template<typename T>
T max(T a, T b)
{
    if (a > b)
        return a;
    return b;
}

uint8_t score_ED(const char &s, const char &r)
{
    if (s == r)
        return 0;
    return 1;
}

ED_data_t edit_distance(const std::string primer, std::string sequence, uint16_t k)
{
    uint32_t rows = sequence.length() + 1, cols = primer.length() + 1;
    matrix_t<uint32_t> *matrix = new matrix_t<uint32_t>(rows, cols);
    uint32_t infinity = matrix->get_infinity();
    uint32_t start_bt_col = cols - 1;
    for (uint32_t col = 2; col < cols; ++col)
        matrix->set(0, col, infinity);
    
    
    for (int col = 1; col < cols; ++col)
    {
        if (col + k + 1 >= cols)
            break;
        if (col >= rows)
        {
            start_bt_col = col + k - 1;
            break;
        }
        matrix->set(col, col + k + 1, infinity);
    }
    
    uint32_t start_row = 1;
    if (cols < rows)
        start_row = max((uint32_t)1, rows - (cols - 1) - k);
    if (start_row > 1)
        for (int col = 1; col < cols; ++col)
            matrix->set(start_row - 1, col, infinity);
    
    bool still_valid {false};
    uint32_t inner_col {1};
    
    for (int row = start_row; row < rows; ++row)
    {
        for (int col = inner_col; col < min((int)cols, row + k + 1); ++col)
        {
            matrix->set(row, col, min(matrix->at(row - 1, col) + 1,
                                     min(matrix->at(row, col - 1) + 1,
                                         matrix->at(row - 1, col - 1) + score_ED(primer.at(col - 1), sequence.at(row - 1)))));
        }
        
        for (int inner_row = row + 1; inner_row < rows; ++inner_row)
        {
            matrix->set(inner_row, inner_col, min(matrix->at(inner_row - 1, inner_col) + 1,
                                                 min(matrix->at(inner_row, inner_col - 1) + 1,
                                                     matrix->at(inner_row - 1, inner_col - 1) + score_ED(primer.at(inner_col - 1), sequence.at(inner_row - 1)))));
        }
        
        for (int col = inner_col; col < min((int)cols, row + k + 1); ++col)
            if (matrix->at(row, col) <= k)
            {
                still_valid = true;
                break;
            }
        for (int inner_row = row + 1; inner_row < rows; ++inner_row)
            if (matrix->at(inner_row, inner_col) <= k)
            {
                still_valid = true;
                break;
            }

        if (!still_valid)
        {
            start_bt_col = inner_col;
            break;
        }
        ++inner_col;
        still_valid = false;
    }
                    
    return ED_data_t {matrix, start_bt_col};
}

BT_data_t back_track(matrix_t<uint32_t> *matrix, uint32_t row, uint32_t col, const std::string primer, const std::string sequnce)
{
    uint32_t hits {0};
    //std::cout << primer << " " << primer.length() << " " << col - 1 << std::endl;
    //std::cout << sequnce << " " << sequnce.length() << " " << row - 1 << std::endl;
    while (true)
    {
        uint8_t hit = score_ED(primer.at(col - 1), sequnce.at(row - 1));
        if (matrix->at(row, col) == matrix->at(row - 1, col - 1) + hit)
        {
            if (hit == 0)
                ++hits;
            --row;
            --col;
        }
        else if (matrix->at(row, col) == matrix->at(row, col - 1) + 1)
            --col;
        else if (matrix->at(row, col) == matrix->at(row - 1, col) + 1)
            --row;
        else
            exit(55);
        if (col <= 1)
        {
            if (col < 1)
                exit(56);
            break;
        }
        if (row < 1)
            exit(57);
    }
    if (row == 0)
        row++;
    return BT_data_t {hits, row - 1,  /*Suffix*/};
}

void task2(const std::string primer, double percent, std::string filepath)
{
    std::set<std::pair<uint32_t, uint32_t>> set_of_sequence_suffix_pairs;
    
    std::ifstream in_fd;
    in_fd.open(filepath);
    
    if (!in_fd.is_open())
        exit(44);
    std::string sequence;
    std::vector<uint32_t> length_of_matching_sequences {};
    uint64_t number_of_sequences {0};
    std::vector<std::string> sequences {};
    while (std::getline(in_fd, sequence))
    {
        if (sequence.empty())
            exit(5); //This should not happen
        ++number_of_sequences;
    
        sequences.emplace_back(sequence);
    }
    in_fd.close();
    
    uint16_t k = (uint16_t)(primer.length() * percent);
    
    uint32_t count {0};
    //#pragma omp parallel for
    for (std::string s : sequences)
    {
        ED_data_t data_ED /*matrix, start_bt_col*/ = edit_distance(primer, s, k);
        //std::cout << data_ED.matrix << std::endl;
        for (int col = data_ED.start_bt_col; col > 1; --col)
        {
            uint32_t row = data_ED.matrix->rows - 1;
            if (data_ED.matrix->at(row, col) > k)
                continue;
            if (count == 1280)
                count;
            BT_data_t data_BT /*hits, suffix*/ = back_track(data_ED.matrix, row, col, primer, s);
            if (data_BT.hits > 3)
            {
                set_of_sequence_suffix_pairs.insert(std::make_pair<uint32_t, uint32_t>(count - 1, data_BT.suffix));
                while (length_of_matching_sequences.size() <= data_BT.suffix)
                    length_of_matching_sequences.emplace_back(0);
                length_of_matching_sequences[data_BT.suffix] += 1;
                break;
            }
        }
    }
    
    
    
    std::ofstream  out_fd;
    //std::string filename = "../../../data/task2_data_" + std::to_string((uint16_t)(percent * 100)) + ".csv";
    std::string filename = "data/task2_data_" + std::to_string((uint16_t)(percent * 100)) + ".csv";
    out_fd.open(filename);
    
    if (!out_fd.is_open())
        exit(45);
    
    for (int i = 0; i < length_of_matching_sequences.size(); ++i)
    {
        out_fd << length_of_matching_sequences[i];
        out_fd << ",";
    }
    out_fd << number_of_sequences;
    out_fd.close();
    std::cout << "Number of sequences: " << number_of_sequences << std::endl;
    
    //std::string filename = "../../../data/task2_data_" + std::to_string((uint16_t)(percent * 100)) + ".csv";
    filename = "data/task2_data_" + std::to_string((uint16_t)(percent * 100)) + "_suffix.csv";
    out_fd.open(filename);
    
    if (!out_fd.is_open())
        exit(45);
    
    std::cout << "Writing set of <sequence number, suffix number>" << std::endl;
    for (auto itr = set_of_sequence_suffix_pairs.begin(); itr != set_of_sequence_suffix_pairs.end(); itr++)
    {
        out_fd << (*itr).first << "," << (*itr).second << "\n";
    }
    out_fd << number_of_sequences;
    out_fd.close();
}

int main()
{
    std::string a = "TGGAATTCTCGGGTGCCAAGGAACTCCAGTCACACAGTGATCTCGTATGCCGTCTTCTGCTTG";
    
    
    gettimeofday(&t_start, NULL);
    //task2(a, 0.1, "../../../data/s_3_sequence_1M.txt");
    task2(a, 0.1, "data/s_3_sequence_1M.txt");
    
    gettimeofday(&t_stop, NULL);
    t_total = WALLTIME(t_stop) - WALLTIME(t_start);
    printf("%.2lf seconds total runtime 10\n", t_total);
    
    
    gettimeofday(&t_start, NULL);
    //task2(a, 0.25, "../../../data/s_3_sequence_1M.txt");
    task2(a, 0.25, "data/s_3_sequence_1M.txt");
    
    gettimeofday(&t_stop, NULL);
    t_total = WALLTIME(t_stop) - WALLTIME(t_start);
    printf("%.2lf seconds total runtime 25\n", t_total);
    
    return EXIT_SUCCESS;
}
