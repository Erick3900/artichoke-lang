#pragma once

#include "error.hpp"

#include "parser/ast.hpp"

namespace arti::lang {

    struct function {
        ast_node::ptr body;
        std::shared_ptr<scope> env;
        std::list<std::string> parameters;

        function() = default;
        ~function() = default;

        function(function &&) = default;
        function(const function &);

        function &operator=(function &&) = default;
        function &operator=(const function &);
    };

}
