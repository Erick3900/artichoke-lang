#pragma once

#include <string>
#include <variant>

#include "error.hpp"
#include "utils/coroutines.hpp"

#include "lexer/token/token.hpp"

namespace arti::lang {

    struct lexer {
        using value_type = expected_t<token>;

        using generator = coro::generator<expected_t<token>>;

        using iterator = generator::iterator;
        using iterator_sentinel = generator::iterator_sentinel;

        std::size_t line;
        std::size_t column;

        std::string source_code;

        lexer(std::string source_code);

        void forward();
        value_type next_token();
        value_type current_token();

        generator &getGenerator();

        iterator begin();
        iterator_sentinel end();

        lexer() = delete;

        ~lexer() = default;

        lexer(lexer &&) = default;
        lexer(const lexer &) = delete;

        lexer &operator=(lexer &&) = default;
        lexer &operator=(const lexer &) = delete;

      private:
        generator init();

        std::string::iterator skip_whitespace(std::string::iterator iter);

        expected_t<std::string> read_char(std::string::iterator &iter);
        expected_t<std::string> read_string(std::string::iterator &iter);
        expected_t<std::pair<std::string, bool>> read_number(std::string::iterator &iter);

        struct op_trie {
            struct node {
                bool is_end;
                token_value op;
                std::unordered_map<char, node> childs;
            };

            node root;

            void insert(std::string_view str, token_value op);
        };

        op_trie ops;
        std::unordered_map<std::string, token_value> keywords;

        token new_eof() const;
        token new_simple(token_type type) const;
        token new_char(std::string_view value) const;
        token new_string(std::string_view value) const;
        token new_decimal(std::string_view value) const;
        token new_integer(std::string_view value) const;
        token new_identifier(std::string_view value) const;
        token new_fallthrough(std::string value) const;

        token new_op(token_value value) const;
        token new_keyword(token_value value) const;
        token lookup_identifier(std::string value) const;

        token match(std::string::iterator &iter);

        void build_op_trie();

        iterator tokens_iterator;
        generator tokens_generator;
    };

}
