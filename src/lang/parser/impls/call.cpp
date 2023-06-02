#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_call(parser &self, ast::ptr left) {
        auto new_node = ast::new_node();
        new_node->data = ast::call{};
        auto &call = std::get<ast::call>(new_node->data);
        call.function = std::move(left);

        if (not self.peek_expect_and_next(token_type::rparen)) {
            do {
                if (auto expected = self.next_token(); not expected) {
                    return error_t{ std::move(expected).error() };
                }

                auto expected_value = self.parse_statement(0);

                if (not expected_value) {
                    return expected_value;
                }

                call.arguments.emplace_back(std::move(expected_value).value());
            } while (self.peek_expect_and_next(token_type::comma));

            if (auto expected = self.peek_expect_and_next(token_type::rparen); not expected) {
                return error_t{ std::move(expected).error() };
            }
        }

        return new_node;
    }

}
