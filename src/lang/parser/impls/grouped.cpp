#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_grouped(parser &self) {
        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_expression = self.parse_statement(0);

        if (not expected_expression) {
            return expected_expression;
        }

        if (auto expected = self.peek_expect_and_next(token_type::rparen); not expected) {
            return error_t{ std::move(expected).error() };
        }

        return std::move(expected_expression).value();
    }

}
