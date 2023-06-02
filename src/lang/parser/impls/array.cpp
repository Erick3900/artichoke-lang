#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_array(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::array{};
        auto &array = std::get<ast::array>(new_node->data);

        if (not self.peek_expect_and_next(token_type::rbracket)) {
            do {
                if (auto expected = self.next_token(); not expected) {
                    return error_t{ std::move(expected).error() };
                }

                auto expected_expression = self.parse_statement(0);

                if (not expected_expression) {
                    return expected_expression;
                }

                array.elements.emplace_back(std::move(expected_expression).value());
            } while (self.peek_expect_and_next(token_type::comma));

            if (auto expected = self.peek_expect_and_next(token_type::rbracket); not expected) {
                return error_t{ std::move(expected).error() };
            }
        }

        return new_node;
    }

}
