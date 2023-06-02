#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_index(parser &self, ast::ptr left) {
        auto new_node = ast::new_node();
        new_node->data = ast::index{};
        auto &index = std::get<ast::index>(new_node->data);

        index.variable = std::move(left);

        if (not self.peek_expect_and_next(token_type::rbracket)) {
            if (auto ok = self.next_token(); not ok) {
                return error_t{ std::move(ok).error() };
            }

            auto expected_value = self.parse_statement(0);

            if (not expected_value) {
                return expected_value;
            }

            index.index = std::move(expected_value).value();

            if (auto expected = self.peek_expect_and_next(token_type::rbracket); not expected) {
                return error_t{ std::move(expected).error() };
            }
        }
        else return error_t{ "Expected index value" };

        return new_node;
    }

}
