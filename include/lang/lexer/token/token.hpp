#pragma once

#include <variant>

#include "types.hpp"
#include "lexer/token/type.hpp"

namespace arti::lang {

    struct token {
        std::size_t line;
        std::size_t column;

        token_type type;
        std::variant<token_value, types::string_t> token;

        std::string to_string() const;
    };

}
