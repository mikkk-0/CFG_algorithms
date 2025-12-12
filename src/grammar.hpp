#pragma once

#include <cctype>
#include <istream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <map>

typedef std::string term_t;
typedef std::string nterm_t;
typedef std::string symbol_t;

struct RuleException: std::exception {
};

struct Rule {
    nterm_t left;
    std::vector<symbol_t> right;
    size_t len;
    size_t rule_i;
    Rule(std::vector<symbol_t> rule);
    auto operator<=>(const Rule&) const = default;
};

struct Grammar {
    std::map<nterm_t, std::vector<Rule>> rules;
    std::vector<Rule> rule_i;
    nterm_t start = "S";

    std::set<term_t> terms;
    std::set<nterm_t> nterms;

    void add_rule(Rule rule);

    bool is_term(symbol_t symbol);

    bool is_nterm(symbol_t symbol);

    void set_start(nterm_t st);

    static symbol_t to_symbol(symbol_t symbol);

    static std::string to_string(symbol_t symbol);

    static term_t to_term(symbol_t symbol);

    static std::string to_string(std::vector<symbol_t> symbols);

    static std::vector<symbol_t> concat(const std::vector<symbol_t>& a, const std::vector<symbol_t>& b);

    static std::vector<symbol_t> to_symbols(std::string s);

    static std::vector<symbol_t> to_symbols(std::vector<term_t> s);
};

Grammar read_grammar(const std::string& filename);