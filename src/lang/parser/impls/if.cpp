#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_if(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::if_else{};
        auto &if_else = std::get<ast::if_else>(new_node->data);

        if (auto expected = self.peek_expect_and_next(token_type::lparen); not expected) {
            return error_t{ std::move(expected).error() };
        }

        if (self.peek_expect_and_next(token_type::rparen)) {
            return error_t{ "Expected if condition" };
        }

        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_condition = self.parse_statement(0);

        if (not expected_condition) {
            return expected_condition;
        }

        if_else.condition = std::move(expected_condition).value();

        if (auto expected = self.peek_expect_and_next(token_type::rparen); not expected) {
            return error_t{ std::move(expected).error() };
        }

        if (auto expected = self.peek_expect_and_next(token_type::lsquirly); not expected) {
            return error_t{ std::move(expected).error() };
        }

        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_if_branch = self.parse_statements_until(token_type::rsquirly);

        if (not expected_if_branch) {
            return expected_if_branch;
        }

        if_else.if_branch = std::move(expected_if_branch).value();

        if (
            self.peek_token.type == token_type::keyword and
            std::get<token_value>(self.peek_token.token) == token_value::k_else
        ) {
            if (auto expected = self.next_token(); not expected) {
                return error_t{ std::move(expected).error() };
            }

            if (auto expected = self.peek_expect_and_next(token_type::lsquirly); not expected) {
                return error_t{ std::move(expected).error() };
            }

            if (auto expected = self.next_token(); not expected) {
                return error_t{ std::move(expected).error() };
            }

            auto expected_else_branch = self.parse_statements_until(token_type::rsquirly);

            if (not expected_else_branch) {
                return expected_else_branch;
            }

            if_else.else_branch = std::move(expected_else_branch).value();
        }

        return new_node;
    }

}
