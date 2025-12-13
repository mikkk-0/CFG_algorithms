#include "lrk.hpp"
#include <algorithm>
#include <iostream>
#include <queue>

using std::size_t;

std::vector<term_t> plus(const std::vector<term_t>& a, const std::vector<term_t>& b, size_t k) {
    if (a.size() >= k) {
        return {a.begin(), a.begin() + k}; 
    }
    std::vector<term_t> res;
    if (a.size() + b.size() > k) {
        res.resize(k);
    } else {
        res.resize(a.size() + b.size());
    }
    for (size_t i = 0; i < res.size(); ++i) {
        if (i < a.size()) {
            res[i] = a[i];
        } else {
            res[i] = b[i - a.size()];
        }
    }
    return res;
}

First plus(const First& a, const First& b, size_t k) {
    First nw;
    for (auto& w1: a.words) {
        for (auto& w2: b.words) {
            nw.words.emplace(plus(w1, w2, k));
        }
    }
    for (auto& w1: a.eps_free) {
        for (auto& w2: b.words) {
            nw.eps_free.emplace(plus(w1, w2, k));
        }
    }
    return nw;
}

void LangRecogn::count_first(const symbol_t& symbol, size_t i, size_t k) {
    if (first_list[i].contains(symbol)) {
        return;
    }
    if (g.is_term(symbol)) {
        First cur;
        cur.words.emplace(1, g.to_term(symbol));
        cur.eps_free.emplace(1, g.to_term(symbol));
        first_list[i][symbol] = cur;
        return;
    }
    if (i == 0) {
        First res;
        for (auto rule: g.rules[symbol]) {
            if (rule.right.size() == 1) {
                res.words.emplace();
                continue;
            }
            if (std::all_of(rule.right.begin(), rule.right.begin() + k,
                [&g=g](symbol_t s) {
                    return g.is_term(s);
                })) {
                std::vector<term_t> w(rule.right.begin(), rule.right.begin() + k);
                res.words.emplace(w);
                res.eps_free.emplace(w);
            }
        }
        first_list[i][symbol] = res;
        return;
    }
    First res = first_list[i - 1][symbol];
    for (const auto& rule: g.rules[symbol]) {
        if (rule.right.size() == 1) {
            res.words.emplace();
            continue;
        }
        auto tos = rule.right;
        First cur;
        bool first = true;
        for (const auto& el : tos) {
            if (el == "$") {
                break;
            }
                count_first(el, i - 1, k);
            if (!first) {
                cur = plus(cur, first_list[i - 1][el], k);
            } else {
                cur = first_list[i - 1][el];
            }
            first = false;
        }
        for (auto& w : cur.words) {
            res.words.emplace(w);
        }
        for (auto& w : cur.eps_free) {
            res.eps_free.emplace(w);
        }
    }
    first_list[i].emplace(symbol, res);
}

First LangRecogn::get_first(const symbol_t& symbol, size_t k) {
    if (first_k.count(symbol)) {
        return first_k[symbol];
    }
    count_first(symbol, 0, k);
    First c = first_list[0][symbol];
    for (size_t i = 1; ; ++i) {
        count_first(symbol, i, k);
        First nw = first_list[i][symbol];
        if (nw == c) {
            break;
        }
        for (auto& w : nw.words) {
            c.words.emplace(w);
        }
        for (auto& w : nw.eps_free) {
            c.eps_free.emplace(w);
        }
        first_list[i][symbol] = c;
    }
    First fk;
    // truncate words
    for (auto& w: c.words) {
        fk.words.emplace(plus(w, {}, k));
    }
    for (auto& w: c.eps_free) {
        fk.eps_free.emplace(plus(w, {}, k));
    }
    return first_k[symbol] = fk;
}

First LangRecogn::get_first(const std::vector<symbol_t>& context, size_t k) {
    First result;
    if (context.size() == 1) {
        result.words.emplace();
        result.eps_free.emplace();
        return result;
    }
    bool first = true;
    for (auto& el : context) {
        if (el == "$") {
            break;
        }
        if (!first) {
            result = plus(result, get_first(el, k), k);
        } else {
            result = get_first(el, k);
        }
        first = false;
    }
    First fk;
    // truncate words
    for (auto& w: result.words) {
        fk.words.emplace(plus(w, {}, k));
    }
    for (auto& w: result.eps_free) {
        fk.eps_free.emplace(plus(w, {}, k));
    }
    return result;
}

void LangRecogn::print_situation(const Situation& el) {
    std::cout << "[" << el.rule.left << "->";
    std::cout << g.to_string(std::vector<symbol_t>{el.rule.right.begin(), el.rule.right.begin() + el.dot_pos}) << ".";
    std::cout << g.to_string(std::vector<symbol_t>{el.rule.right.begin() + el.dot_pos, el.rule.right.end()}) << ", ";
    std::cout << g.to_string(g.to_symbols(el.lookup)) << "]\n";
}

void LangRecogn::predict(SituationList& list, size_t k) {
    while (true) {
        std::vector<nterm_t> pred_cand; // candidates for predict
        for (const auto& st: list) {
            symbol_t symbol = st.rule.right[st.dot_pos];
            if (symbol == "$") {
                continue;
            }
            if (g.is_term(symbol)) {
                continue;
            }
            pred_cand.push_back(symbol);
        }
        bool changed = false;
        std::set<Situation> news;
        for (auto& symbol: pred_cand) {
            for (auto& s: list) {
                if (s.rule.right[s.dot_pos] != symbol) {
                    continue;
                }
                std::vector<symbol_t> after_nterm(s.rule.right.begin() + s.dot_pos + 1, s.rule.right.end());
                std::vector<symbol_t> lookup = g.concat(after_nterm, g.to_symbols(s.lookup));
                First us = get_first(lookup, k);
                for (const auto& rule: g.rules[symbol]) {
                    for (const auto& w: us.words) {
                        Situation nw(rule, 0, w);
                        news.emplace(nw);
                    }
                }
            }
        }
        for (auto& nw: news) {
            symbol_t where = nw.rule.right[0];
            if (list.contains(nw)) {
                continue;
            }
            changed = true;
            list.emplace(nw);
        }
        if (!changed) {
            break;
        }
    }
}

LangRecogn::SituationList LangRecogn::go_to(SituationList list, const symbol_t& next, size_t k) {
    SituationList res;
    for (auto situation: list) {
        if (situation.rule.right[situation.dot_pos] != next) {
            continue;
        }
        situation.dot_pos++;
        res.emplace(situation);
    }
    predict(res, k);
    return res;
}

LangRecogn::SituationList LangRecogn::init_auto(size_t k) {
    SituationList list;
    auto rule = g.rule_i.back();
    list.insert(Situation(g.rule_i.back(), 0, {}));
    predict(list, k);
    return list;
}

void LangRecogn::init(const Grammar& gr) {
    g = gr;
    size_t k = 0;
    std::vector<std::vector<term_t>> lookups;
    lookups.push_back({});
    while (true) {
        try {
            size_t sz = lookups.size();
            for (size_t i = 0; i < sz; ++i) {
                if (lookups[i].size() + 1 == k) {
                    std::vector nw = lookups[i];
                    for (const auto& t : g.terms) {
                        nw.emplace_back(t);
                        lookups.emplace_back(nw);
                        nw.pop_back();
                    }
                }
            }
            build(lookups, k);
            this->k_ = k;
            return;
        } catch(int x) {
            first_list.clear();
            first_k.clear();
            mpa_f.clear();
            mpa_g.clear();
            sts.clear();
            st_i.clear();
            k++;
        }
    }
}

void LangRecogn::build(auto& lookups, size_t k) {
    // build set of good sets
    std::vector<std::vector<symbol_t>> list_to_stack;
    std::map<std::vector<symbol_t>, size_t> stack_to_list;
    std::set<SituationList> lists;
    {
        std::queue<std::pair<size_t, std::vector<symbol_t>>> q;
        sts.push_back(init_auto(k));
        lists.emplace(sts[0]);
        q.emplace(0, std::vector<symbol_t>{});
        st_i[sts[0]] = 0;
        list_to_stack.emplace_back();
        stack_to_list[{}] = 0;
        mpa_g.emplace_back();
        while (!q.empty()) {
            auto [cur, stack] = q.front();
            q.pop();
            for (auto& let : g.nterms) {
                auto s = go_to(sts[cur], let, k);
                if (!s.empty()) {
                stack.push_back(let);
                if (!lists.contains(s)) {
                    size_t ind = sts.size();
                    st_i[s] = ind;
                    sts.push_back(s);
                    mpa_g.emplace_back();
                    list_to_stack.emplace_back(stack);
                    stack_to_list[stack] = ind;
                    lists.emplace(s);
                    q.emplace(ind, stack);
                }
                mpa_g[cur][let] = st_i[s];
                stack.pop_back();
                }
            }
            for (auto& let : g.terms) {
                auto s = go_to(sts[cur], let, k);
                if (!s.empty()) {
                stack.push_back(let);
                if (!lists.contains(s)) {
                    size_t ind = sts.size();
                    st_i[s] = ind;
                    sts.push_back(s);
                    mpa_g.emplace_back();
                    list_to_stack.emplace_back(stack);
                    stack_to_list[stack] = ind;
                    lists.emplace(s);
                    q.emplace(ind, stack);
                }
                mpa_g[cur][let] = st_i[s];
                stack.pop_back();
                }
            }
        }
    }
    mpa_f.resize(sts.size());
    // build automaton
    for (size_t i = 0; i < sts.size(); ++i) {
        const auto& list = sts[i];
        for (auto& l : lookups) {
            const Situation* to_go = nullptr;
            const Situation* to_reduce = nullptr;
            for (const auto& s: list) {
                if (s.lookup == l && s.dot_pos + 1 == s.rule.len) {
                    if (to_reduce != nullptr) {
                        throw -1;
                    }
                    to_reduce = &s;
                    continue;
                }
                auto where = g.concat(
                    std::vector<symbol_t>{
                    s.rule.right.begin() + s.dot_pos,
                    s.rule.right.end()
                    },
                    s.lookup);
                First fst = get_first(where, k);
                if (fst.eps_free.contains(l)) {
                    to_go = &s;
                }
            }
            if (to_reduce != nullptr && to_go != nullptr) {
                throw 0;
            }
            pass cur;
            if (to_go == nullptr && to_reduce == nullptr) {
                cur.op = pass::Error;
            } else if (to_go == nullptr) {
                cur.op = pass::Reduce;
                cur.reduce_rule = to_reduce->rule.rule_i;
            } else {
                cur.op = pass::Shift;
            }
            if (to_reduce != nullptr && l.empty() && to_reduce->rule == g.rule_i.back() && to_reduce->dot_pos == 1) {
                cur.op = pass::Accept;
            }
            mpa_f[i][l] = cur;
        }
    }
    // success!
}

bool LangRecogn::proceed(const std::vector<term_t>& word) {
    std::vector<symbol_t> stack;
    size_t pos = 0;
    std::vector<size_t> states;
    states.emplace_back();
    while (true) {
        size_t left = pos + k_ <= word.size() ? k_ : word.size() - pos;
        std::vector<term_t> lookup(word.begin() + pos, word.begin() + pos + left);
        size_t cur = states.back();
        pass go = mpa_f[cur][lookup];
        if (go.op == pass::Error) {
            return false;
        }
        if (go.op == pass::Accept) {
            return pos == word.size();
        }
        if (go.op == pass::Shift) {
            if (pos == word.size()) {
                return false;
            }
            stack.push_back(word[pos++]);
        }
        if (go.op == pass::Reduce) {
            size_t len = g.rule_i[go.reduce_rule].len;
            stack.resize(stack.size() + 1 - len);
            stack.push_back(g.rule_i[go.reduce_rule].left);
            states.resize(states.size() + 1 - len);
        }
        states.push_back(mpa_g[states.back()][stack.back()]);
    }
    return false;
}