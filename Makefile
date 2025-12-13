lrk: build/ build/lrk copy-tests	
	cd build && ./lrk lrk grammar.txt tests.txt

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
	cp tests/*.txt build/

build/gtest-all.o: third_party/googletest/googletest/src/gtest-all.cc
	g++ -std=c++2b -O0 -g -Ithird_party/googletest/googletest/include -Ithird_party/googletest/googletest -c third_party/googletest/googletest/src/gtest-all.cc -o build/gtest-all.o

build/gtest_main-all.o: third_party/googletest/googletest/src/gtest_main.cc
	g++ -std=c++2b -O0 -g -Ithird_party/googletest/googletest/include -Ithird_party/googletest/googletest -c third_party/googletest/googletest/src/gtest_main.cc -o build/gtest_main-all.o

build/test_lrk.test.o: tests/unit/test_lrk.cpp src/grammar.hpp src/lrk.hpp src/analyzer.hpp build/
	g++ -std=c++2b -O0 -g -Isrc -Ithird_party/googletest/googletest/include -c tests/unit/test_lrk.cpp -o build/test_lrk.test.o

build/test_lrk: build/ build/gtest-all.o build/gtest_main-all.o build/test_lrk.test.o build/lrk.o build/grammar.o
	g++ -std=c++2b -O0 -g -Isrc build/test_lrk.test.o build/lrk.o build/grammar.o build/gtest-all.o build/gtest_main-all.o -lpthread -o build/test_lrk

test: build/test_lrk copy-tests
	cd build && ./test_lrk

clean:
	rm -rf build