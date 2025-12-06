run-earley: build/ build/earley copy-tests	
	build/earley earley build/grammar.txt build/tests.txt

build/earley: build/ build/main.o build/earley.o build/grammar.o
	g++ -std=c++2b -O3 build/main.o build/earley.o build/grammar.o -o build/earley

build/main.o: src/main.cpp src/grammar.hpp src/analyzer.hpp src/earley.hpp
	g++ -std=c++2b -O3 -c src/main.cpp -o build/main.o

build/earley.o: src/earley.cpp src/earley.hpp src/analyzer.hpp
	g++ -std=c++2b -O3 -c src/earley.cpp -o build/earley.o

build/grammar.o: src/grammar.cpp src/grammar.hpp
	g++ -std=c++2b -O3 -c src/grammar.cpp -o build/grammar.o

build/:
	mkdir -p build

copy-tests:
	cp tests/grammar.txt build/grammar.txt
	cp tests/tests.txt build/tests.txt

clean:
	rm -rf build