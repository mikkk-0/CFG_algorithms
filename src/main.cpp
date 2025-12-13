#include <iostream>
#include "lrk.hpp"
#include "grammar.hpp"
#include "analyzer.hpp"

int main(int argc, char** argv) {
    Analyzer* analyzer;
    std::string type = argv[1];
    if (type == "lrk") {
        analyzer = new LangRecogn;
    }
    std::string gram = argv[2];
    std::string inputs = argv[3];
    Grammar g = read_grammar(gram);
    analyzer->init(g);
    auto words = read_tests(inputs);
    for (auto& w: words) {
        std::cout << (analyzer->proceed(w) ? "YES" : "NO") << std::endl;
    }
    delete analyzer;
    return 0;
}