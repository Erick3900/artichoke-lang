#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_fn(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::function{};
        auto &fn = std::get<ast::function>(new_node->data);

        if (auto expected = self.peek_expect_and_next(token_type::lparen); not expected) {
            return error_t{ std::move(expected).error() };
        }

        if (not self.peek_expect_and_next(token_type::rparen)) {
            do {
                if (auto expected = self.peek_expect_and_next(token_type::identifier); not expected) {
                    return error_t{ std::move(expected).error() };
                }
                fn.parameters.push_back(std::get<types::string_t>(self.current_token.token));
            } while (self.peek_expect_and_next(token_type::comma));

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

        auto expected_block = self.parse_statements_until(token_type::rsquirly);

        if (not expected_block) {
            return expected_block;
        }

        fn.body = std::move(expected_block).value();

        return new_node;
    }

}
