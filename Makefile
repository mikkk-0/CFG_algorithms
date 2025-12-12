run-lrk: build/ build/lrk copy-tests	
	build/lrk lrk build/grammar.txt build/tests.txt

build/lrk: build/ build/main.o build/lrk.o build/grammar.o
	g++ -std=c++2b -O3 build/main.o build/lrk.o build/grammar.o -o build/lrk

build/main.o: src/main.cpp src/grammar.hpp src/analyzer.hpp src/lrk.hpp
	g++ -std=c++2b -O3 -c src/main.cpp -o build/main.o

build/lrk.o: src/lrk.cpp src/lrk.hpp src/analyzer.hpp
	g++ -std=c++2b -O3 -c src/lrk.cpp -o build/lrk.o

build/grammar.o: src/grammar.cpp src/grammar.hpp
	g++ -std=c++2b -O3 -c src/grammar.cpp -o build/grammar.o

build/:
	mkdir -p build

copy-tests:
	cp tests/grammar.txt build/grammar.txt
	cp tests/tests.txt build/tests.txt

clean:
	rm -rf build