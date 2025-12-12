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

build/gtest: build/
	mkdir -p build/gtest

build/gtest/gtest_main.o: build/gtest third_party/googletest/googletest/src/gtest_main.cc
	g++ -std=c++2b -O3 -isystem third_party/googletest/googletest/include -Ithird_party/googletest -c third_party/googletest/googletest/src/gtest_main.cc -o build/gtest/gtest_main.o

build/tests/unit/test_earley.o: build/gtest tests/unit/test_earley.cpp src/earley.hpp
	mkdir -p build/tests/unit
	g++ -std=c++2b -O3 -isystem third_party/googletest/googletest/include -Isrc -Ithird_party/googletest -c tests/unit/test_earley.cpp -o build/tests/unit/test_earley.o

build/run_unit_tests: build/gtest build/gtest/gtest_main.o build/tests/unit/test_earley.o build/earley.o build/grammar.o
	g++ -std=c++2b -O3 build/gtest/gtest_main.o build/tests/unit/test_earley.o build/grammar.o build/earley.o \
		-o build/run_unit_tests -pthread

test: build/run_unit_tests
	./build/run_unit_tests

copy-tests:
	cp tests/grammar.txt build/grammar.txt
	cp tests/tests.txt build/tests.txt

clean:
	rm -rf build