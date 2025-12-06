#include "grammar.hpp"

struct Analyzer {
    virtual void init(const Grammar&) = delete;
    virtual bool proceed(const std::vector<term_t>&) = delete;
};