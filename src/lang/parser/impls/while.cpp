#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_while(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::while_loop{};
        auto &while_loop = std::get<ast::while_loop>(new_node->data);

        if (auto expected = self.peek_expect_and_next(token_type::lparen); not expected) {
            return error_t{ std::move(expected).error() };
        }

        if (self.peek_expect_and_next(token_type::rparen)) {
            return error_t{ "Expected while condition" };
        }

        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_condition = self.parse_statement(0);

        if (not expected_condition) {
            return expected_condition;
        }

        while_loop.condition = std::move(expected_condition).value();

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

        while_loop.body = std::move(expected_if_branch).value();

        return new_node;
    }

}
