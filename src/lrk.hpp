#pragma once

#include "grammar.hpp"
#include "analyzer.hpp"

using std::size_t;

struct First {
  std::set<std::vector<term_t>> words;
  std::set<std::vector<term_t>> eps_free;
  auto operator<=>(const First&) const = default;
};

std::vector<term_t> plus(const std::vector<term_t>& a, const std::vector<term_t>& b, size_t k);

First plus(const First& a, const First& b, size_t k);

class LangRecogn: public Analyzer {
  Grammar g;

  std::map<size_t, std::map<symbol_t, First>> first_list;
  std::map<symbol_t, First> first_k;

  size_t k_;

  struct pass {
    enum {
      Accept,
      Error,
      Shift,
      Reduce
    } op;
    union {
      size_t reduce_rule;
    };
  };

  struct Situation {
    Rule rule;
    size_t dot_pos;
    std::vector<term_t> lookup;
    auto operator<=>(const Situation& other) const = default;
  };

  using SituationList = std::set<Situation>;

  std::map<SituationList, size_t> st_i;
  std::vector<SituationList> sts;
  std::vector<std::map<std::vector<term_t>, pass>> mpa_f;
  std::vector<std::map<symbol_t, size_t>> mpa_g;

  void count_first(const symbol_t& symbol, size_t i, size_t k);

  First get_first(const symbol_t& symbol, size_t k);

  First get_first(const std::vector<symbol_t>& context, size_t k);

  void print_situation(const Situation& el);

  void predict(SituationList& list, size_t k);

  SituationList go_to(SituationList list, const symbol_t& next, size_t k);

  SituationList init_auto(size_t k);

  void build(auto& lookups, size_t k);
public:
  void init(const Grammar&) override;

  bool proceed(const std::vector<term_t>& word) override;
};