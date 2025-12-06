#include "grammar.hpp"

bool Grammar::is_term(symbol_t symbol) {
    return terms.contains(symbol);
}

bool Grammar::is_nterm(symbol_t symbol) {
    return nterms.contains(symbol);
}

symbol_t Grammar::to_symbol(symbol_t symbol) {
    return symbol;
}

std::string Grammar::to_string(symbol_t symbol) {
    return symbol;
}

term_t Grammar::to_term(symbol_t symbol) {
    return symbol;
}

std::string Grammar::to_string(std::vector<symbol_t> symbols) {
    std::string z;
    for (auto el : symbols) {
        z += Grammar::to_string(el);
    }
    return z;
}

std::vector<symbol_t> Grammar::concat(const std::vector<symbol_t>& a, const std::vector<symbol_t>& b) {
    std::vector<symbol_t> res(a.size() + b.size());
    std::copy(a.begin(), a.end() - 1, res.begin());
    std::copy(b.begin(), b.end(), res.begin() + a.size() - 1);
    res.back() = "$";
    return res;
}

std::vector<symbol_t> Grammar::to_symbols(std::string s) {
    std::vector<symbol_t> symbols;
    for (auto c : s) {
        symbols.push_back(Grammar::to_symbol(std::string(1, c)));
    }
    return symbols;
}

std::vector<symbol_t> Grammar::to_symbols(std::vector<term_t> s) {
    std::vector<symbol_t> res;
    for (auto& x : s) {
        res.push_back(Grammar::to_symbol(x));
    }
    return res;
}

void Grammar::add_rule(const Rule& rule) {
    if (Grammar::is_term(rule.left)) {
        throw RuleException{};
    }
    rules[rule.left].push_back(rule);
}

void Grammar::set_start(nterm_t st) {
    this->start = st;
    std::vector<symbol_t> starter_rule;
    starter_rule.emplace_back(st + "'");
    starter_rule.emplace_back("->");
    starter_rule.emplace_back(st);
    Rule rule(starter_rule);
    rules[rule.left].push_back(rule);
}

Grammar read_grammar(const std::string& filename) {
    std::ifstream in(filename);
    size_t nterm_n, term_n, rule_n;
    in >> nterm_n >> term_n >> rule_n;
    Grammar g;
    while (nterm_n--) {
        nterm_t nterm;
        in >> nterm;
        g.nterms.emplace(nterm);
    }
    while (term_n--) {
        term_t term;
        in >> term;
        g.terms.emplace(term);
    }
    while (rule_n--) {
        std::string line;
        std::getline(in, line);
        while (!line.empty() && isspace(line.back()) && line.back() != '>') {
            line.pop_back();
        }
        if (line.empty()) {
            rule_n++;
            continue;
        }
        std::vector<symbol_t> rule_symbols;
        std::string cur;
        for (char c : line) {
            if (c == ' ') {
                rule_symbols.emplace_back(cur);
                cur = "";
            } else {
                cur += c;
            }
        }
        rule_symbols.emplace_back(cur);
        g.add_rule(rule_symbols);
    }
    nterm_t start;
    in >> start;
    g.set_start(start);
    in.close();
    return g;
}

Rule::Rule(std::vector<symbol_t> rule) {
    static size_t rules_created = 0;
    rule_i = rules_created++;
    size_t n = rule.size();
    size_t i = 0;
    while (i < n && rule[i] != "->") {
        i++;
    }
    if (i == n) {
        throw RuleException{}; 
    }
    if (i != 1) {
        throw RuleException{};
    }
    right = std::vector<symbol_t>(rule.begin() + i + 1, rule.end());
    left = rule[0];
    right.emplace_back("$");
    len = right.size();
}

std::vector<std::vector<term_t>> read_tests(const std::string& inputs) {
    std::ifstream fin(inputs);
    size_t n_words;
    fin >> n_words;
    std::vector<std::vector<term_t>> test;
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
        test.push_back(word);
    }
    fin.close();
    return test;
}