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
    std::ifstream fin(inputs);
    size_t n_words;
    fin >> n_words;
    while (n_words--) {
        std::vector<term_t> word;
        std::string s;
        getline(fin, s);
        while (!s.empty() && isspace(s.back())) {
            s.pop_back();
        }
        if (s.empty()) {
            n_words++;
            continue;
        }
        std::string cur;
        for (auto& c : s) {
            if (!isspace(c)) {
                cur += c;
            } else {
                word.emplace_back(cur);
                cur = "";
            }
        }
        word.emplace_back(cur);
        std::cout << analyzer->proceed(word) << '\n';
    }
    fin.close();
    delete analyzer;
    return 0;
}