

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




if __name__ == '__main__':
    print('hi')