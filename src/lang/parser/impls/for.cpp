#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_for(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::for_loop{};
        auto &for_loop = std::get<ast::for_loop>(new_node->data);

        if (auto expected = self.peek_expect_and_next(token_type::lparen); not expected) {
            return error_t{ std::move(expected).error() };
        }

        if (self.peek_expect_and_next(token_type::rparen)) {
            return error_t{ "Expected for loop syntax" };
        }

        if (not self.peek_expect_and_next(token_type::semicolon)) {
            if (auto ok = self.next_token(); not ok) {
                return error_t{ std::move(ok).error() };
            }

            auto expected_expr = self.parse_statement(0);

            if (not expected_expr) {
                return expected_expr;
            }

            for_loop.before = std::move(expected_expr).value();

            if (auto expected = self.peek_expect_and_next(token_type::semicolon); not expected) {
                return error_t{ std::move(expected).error() };
            }
        }

        if (not self.peek_expect_and_next(token_type::semicolon)) {
            if (auto ok = self.next_token(); not ok) {
                return error_t{ std::move(ok).error() };
            }

            auto expected_expr = self.parse_statement(0);

            if (not expected_expr) {
                return expected_expr;
            }

            for_loop.condition = std::move(expected_expr).value();

            if (auto expected = self.peek_expect_and_next(token_type::semicolon); not expected) {
                return error_t{ std::move(expected).error() };
            }
        }

        if (not self.peek_expect_and_next(token_type::rparen)) {
            if (auto ok = self.next_token(); not ok) {
                return error_t{ std::move(ok).error() };
            }

            auto expected_expr = self.parse_statement(0);

            if (not expected_expr) {
                return expected_expr;
            }

            for_loop.after = std::move(expected_expr).value();

            if (auto expected = self.peek_expect_and_next(token_type::rparen); not expected) {
                return error_t{ std::move(expected).error() };
            }
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

        for_loop.body = std::move(expected_if_branch).value();

        return new_node;
    }

}
