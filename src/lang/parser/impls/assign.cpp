#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_assign(parser &self, ast::ptr left) {
        auto new_node = ast::new_node();
        new_node->data = ast::assign{};
        auto &assign = std::get<ast::assign>(new_node->data);

        assign.variable = std::move(left);

        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_value = self.parse_statement(0);

        if (not expected_value) {
            return expected_value;
        }

        assign.value = std::move(expected_value).value();

        return new_node;
    }

}
