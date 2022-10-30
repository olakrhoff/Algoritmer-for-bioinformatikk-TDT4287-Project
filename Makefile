CC = g++
CFLAGS = -std=c++20 -O3

CURDIR = $(PWD)
.PHONY: setup task1 task2 task4 clean run1 run2 run3 run4 run_all

setup:
	mkdir -p plots data src/bin
	tar xzvf handout/MultiplexedSamples.txt.tar.gz -C data
	tar xzvf handout/tdt4287-unknown-adapter.txt.tar.gz -C data
	cp handout/s_3_sequence_1M.txt data
	cp handout/seqset3.txt data


task1:
	$(CC) $(CFLAGS) $(CURDIR)/src/task1/main.cpp -o $(CURDIR)/src/bin/task1

task2:
	$(CC) $(CFLAGS) $(CURDIR)/src/task2/main.cpp -o $(CURDIR)/src/bin/task2

task4:
	$(CC) $(CFLAGS) $(CURDIR)/src/task4/main.cpp $(CURDIR)/src/task4/keyword_tree.cpp -o $(CURDIR)/src/bin/task4

clean:
	$(ECHO $(CURDIR))
	-rm -f $(CURDIR)/src/bin/*

run1: task1
	./src/bin/task1
	python3 src/task1.py

run2: task2
	./src/bin/task2
	python3 src/task2.py

run3:
	python3 src/task3.py

run4: task4
	./src/bin/task4
	python3 src/task4.py

run_all: clean run1 run2 run3 run4
