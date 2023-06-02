#pragma once

#include <list>
#include <memory>
#include <variant>

#include "types.hpp"
#include "lexer/token/type.hpp"

namespace arti::lang {

    struct ast_node;

    namespace ast {
        using ptr = std::unique_ptr<ast_node>;

        enum class operators {
            bit_or,
            bit_xor,
            bit_and,
            bit_not,

            bit_left_shift,
            bit_right_shift,

            boolean_or,
            boolean_not,
            boolean_and,

            equal,
            not_equal,

            less_than,
            less_than_equal,

            greater_than,
            greater_than_equal,

            addition,
            substraction,

            modulo,
            division,
            multiplication,
        };

        struct hash_pair {
            ptr key;
            ptr value;

            std::string dump(std::string indent) const;
        };

        struct hash {
            std::list<hash_pair> elements;

            std::string dump(std::string indent) const;
        };

        struct array {
            std::list<ptr> elements;

            std::string dump(std::string indent) const;
        };

        struct function {
            ptr body;
            std::list<std::string> parameters;

            std::string dump(std::string indent) const;
        };

        struct unary {
            ptr right;
            operators op;

            std::string dump(std::string indent) const;
        };

        struct binary {
            ptr left;
            ptr right;
            operators op;

            std::string dump(std::string indent) const;
        };

        struct assign {
            ptr variable;
            ptr value;

            std::string dump(std::string indent) const;
        };

        struct declaration {
            ptr value;
            bool is_const;
            std::string variable;

            std::string dump(std::string indent) const;
        };

        struct call {
            ptr function;
            std::list<ptr> arguments;

            std::string dump(std::string indent) const;
        };

        struct index {
            ptr variable;
            ptr index;

            std::string dump(std::string indent) const;
        };

        struct if_else {
            ptr condition;
            ptr if_branch;
            ptr else_branch;

            std::string dump(std::string indent) const;
        };

        struct for_loop {
            ptr body;
            ptr after;
            ptr before;
            ptr condition;

            std::string dump(std::string indent) const;
        };

        struct while_loop {
            ptr body;
            ptr condition;

            std::string dump(std::string indent) const;
        };

        struct return_value {
            ptr value;

            std::string dump(std::string indent) const;
        };

        struct identifier {
            std::string name;

            std::string dump(std::string indent) const;
        };

    }

    struct ast_node {
        using ptr = ast::ptr;

        bool had_semicolon;
        std::variant<
            types::null_t,
            types::bool_t,
            types::char_t,
            types::int_t,
            types::float_t,
            types::string_t,
            ast::hash,
            ast::array,
            ast::function,
            ast::unary,
            ast::binary,
            ast::assign,
            ast::declaration,
            ast::call,
            ast::index,
            ast::if_else,
            ast::for_loop,
            ast::while_loop,
            ast::return_value,
            ast::identifier,
            std::list<ptr>
        > data;

        ast_node() = default;
        ~ast_node() = default;

        ast_node(ast_node &&) = default;
        ast_node(const ast_node &) = delete;

        ast_node &operator=(ast_node &&) = default;
        ast_node &operator=(const ast_node &) = delete;

        std::string dump(std::string indent = "") const;
    };

    namespace ast {

        static inline ptr new_node() {
            return std::make_unique<ast_node>();
        }

    }

}
