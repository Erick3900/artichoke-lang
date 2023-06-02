#pragma once

#include <ranges>
#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include <fmt/args.h>
#include <fmt/ostream.h>

#include "parser/parser.hpp"
#include "evaluator/object.hpp"

namespace arti::lang {

    struct scope {
        std::shared_ptr<scope> outer;
        std::unordered_set<object*> childs;
        std::unordered_map<std::string, object> variables;

        static std::shared_ptr<scope> new_enclosed(std::shared_ptr<scope> outer);

        void set(const std::string &name, object obj);

        expected_t<object> get(const std::string &name); 
        expected_t<std::reference_wrapper<object>> get_ref(const std::string &name);

        bool is_defined(const std::string &name);
    };

}
