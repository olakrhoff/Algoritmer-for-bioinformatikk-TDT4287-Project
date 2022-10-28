

setup:
	mkdir -p plots data
	tar xzvf handout/MultiplexedSamples.txt.tar.gz -C data
	tar xzvf handout/tdt4287-unknown-adapter.txt.tar.gz -C data
	cp handout/s_3_sequence_1M.txt data
	cp handout/seqset3.txt data

