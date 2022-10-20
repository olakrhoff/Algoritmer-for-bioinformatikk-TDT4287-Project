

setup:
	mkdir -p plots data
	tar xzvf handout/MultiplexedSamples.txt.tar.gz
	mv MultiplexedSamples.txt data
	tar xzvf handout/tdt4287-unknown-adapter.txt.tar.gz
	mv tdt4287-unknown-adapter.txt data
