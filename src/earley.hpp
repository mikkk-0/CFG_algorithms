#pragma once

#include "analyzer.hpp"
#include <map>
#include <set>
#include <iostream>

class Earley: public Analyzer {
    struct Situation {
        Rule rule;
        size_t dot_pos;
        size_t pref_pos;
        auto operator<=>(const Situation& other) const = default;
    };

    struct Hash {
        size_t operator() (const Situation& s) const {
            std::string full;
            full += Grammar::to_string(Grammar::to_symbol(s.rule.left));
            full += "->";
            full += Grammar::to_string(s.rule.right);
            full += " ";
            full += std::to_string(s.dot_pos);
            full += " ";
            full += std::to_string(s.pref_pos);
            return std::hash<std::string>{}(full);
        }
    };

    Grammar g;

public:
    void init(const Grammar& gr) override;

    bool proceed(const std::vector<term_t>& str) override;
};
