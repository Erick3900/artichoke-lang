#pragma once

#include "lexer/lexer.hpp"
#include "parser/ast.hpp"

namespace arti::lang {

    struct parser {
        using ast_ptr = ast::ptr;

        using unary_parser_t = std::function<expected_t<ast_ptr>(parser &)>;
        using binary_parser_t = std::function<expected_t<ast_ptr>(parser &, ast_ptr)>;

        std::unique_ptr<lexer> lxr;

        token peek_token;
        token current_token;

        parser(std::string source_code);

        parser() = delete;
        ~parser() = default;

        parser(parser &&) = default;
        parser(const parser &) = delete;

        parser &operator=(parser &&) = default;
        parser &operator=(const parser &) = delete;

        [[nodiscard]] expected_t<void> next_token();

        expected_t<ast_ptr> parse();

        expected_t<ast_ptr> parse_statement(std::size_t precedence);

        expected_t<ast_ptr> parse_statements_until(token_type type);

        expected_t<unary_parser_t> get_unary_callback(const token &tok);
        expected_t<binary_parser_t> get_binary_callback(const token &tok);

        std::size_t peek_precedence();
        std::size_t current_precedence();

        std::size_t precedence(const token &, bool unary = false);

        expected_t<void> expect(token_type type);
        expected_t<void> peek_expect(token_type type);
        expected_t<void> expect_and_next(token_type type);
        expected_t<void> peek_expect_and_next(token_type type);
    };

    expected_t<ast::ptr> parse_array(parser &self);
    expected_t<ast::ptr> parse_assign(parser &self, ast::ptr);
    expected_t<ast::ptr> parse_binary(parser &self, ast::ptr);
    expected_t<ast::ptr> parse_boolean(parser &self);
    expected_t<ast::ptr> parse_call(parser &self, ast::ptr);
    expected_t<ast::ptr> parse_character(parser &self);
    expected_t<ast::ptr> parse_decimal(parser &self);
    expected_t<ast::ptr> parse_declaration(parser &self);
    expected_t<ast::ptr> parse_fn(parser &self);
    expected_t<ast::ptr> parse_for(parser &self);
    expected_t<ast::ptr> parse_grouped(parser &self);
    expected_t<ast::ptr> parse_hash(parser &self);
    expected_t<ast::ptr> parse_identifier(parser &self);
    expected_t<ast::ptr> parse_if(parser &self);
    expected_t<ast::ptr> parse_index(parser &self, ast::ptr);
    expected_t<ast::ptr> parse_integer(parser &self);
    expected_t<ast::ptr> parse_null(parser &self);
    expected_t<ast::ptr> parse_return(parser &self);
    expected_t<ast::ptr> parse_string(parser &self);
    expected_t<ast::ptr> parse_unary(parser &self);
    expected_t<ast::ptr> parse_while(parser &self);

}
