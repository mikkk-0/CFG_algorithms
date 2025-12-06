#include "earley.hpp"

void Earley::init(const Grammar& gr) {
    g = gr;
}

bool Earley::proceed(const std::vector<term_t>& str) {
    size_t n = str.size();
    std::vector<std::map<symbol_t, std::set<Situation>>> d(n + 1);
    auto scan = [&](size_t idx, symbol_t letter) {
        for (auto s : d[idx][letter]) {
            size_t n_pos = s.dot_pos + 1;
            symbol_t next = s.rule.right[n_pos];
            d[idx + 1][next].emplace(s.rule, n_pos, s.pref_pos);
        }
    };
    auto predict = [&](size_t idx) -> bool {
        bool changed = false;
        for (const auto& let : g.nterms) {
            for (const auto& rule : g.rules[let]) {
                if (!d[idx][rule.right[0]].contains({rule, 0, idx})) {
                    changed = true;
                    d[idx][rule.right[0]].emplace(rule, 0, idx);
                }
            }
        }
        return changed;
    };
    auto complete = [&](size_t idx) -> bool {
        bool changed = false;
        for (auto s : d[idx]["$"]) {
            size_t k = s.pref_pos;
            for (auto p : d[k][s.rule.left]) {
                size_t n_pos = p.dot_pos + 1;
                symbol_t next = p.rule.right[n_pos];
                Situation nw{p.rule, n_pos, p.pref_pos};
                if (!d[idx][next].contains(nw)) {
                    changed = true;
                    d[idx][next].insert(nw);
                }
            }
        }
        return changed;
    };
    auto starter = g.rules[g.start + "'"][0];
    d[0][g.start].emplace(starter, 0, 0);
    while (true) {
        bool changed = false;
        changed |= predict(0);
        changed |= complete(0);
        if (!changed) {
            break;
        }
    }
    for (size_t i = 0; i < n; ++i) {
        scan(i, Grammar::to_symbol(str[i]));
        while (true) {
            bool changed = false;
            changed |= predict(i + 1);
            changed |= complete(i + 1);
            if (!changed) {
                break;
            }
        }
    }
    return d[n]["$"].contains({starter, 1});
}