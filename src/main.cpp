#include <iostream>
#include "earley.hpp"
#include "grammar.hpp"
#include "analyzer.hpp"

int main(int argc, char** argv) {
    Analyzer* analyzer;
    std::string type = argv[1];
    if (type == "earley") {
        analyzer = new Earley;
    }
    std::string gram = argv[2];
    std::string inputs = argv[3];
    Grammar g = read_grammar(gram);
    analyzer->init(g);
    auto tests = read_tests(inputs);
    for (auto& w: tests) {
        std::cout << analyzer->proceed(w) << '\n';
    }
    delete analyzer;
    return 0;
}