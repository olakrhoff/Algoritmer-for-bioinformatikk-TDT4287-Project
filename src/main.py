

# Create implicit suffix tree of a (adapter) IS_a
# Loop through all s in S
# Copy IS_a -> IS_ac
# Add s to IS_ac
# Make IS_ac explicit -> ES_ac (note suffix in inner nodes, if leaf node only has $, add to list in parent)
# if parent has prefix leaf node, add solution to list (length_of_sequence(suffix_start)) break loop
# else continue
# goto loop

# Print length of list -> number of sequences found
# Plot normal dist. of values in list

if __name__ == '__main__':
    print('hi')

