#pragma once

#include "grammar.hpp"

struct Analyzer {
    virtual void init(const Grammar&) = 0;
    virtual bool proceed(const std::vector<term_t>&) = 0;
};