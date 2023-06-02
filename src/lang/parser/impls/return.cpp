#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_return(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::return_value{};
        auto &ret = std::get<ast::return_value>(new_node->data);

        if (not self.peek_expect(token_type::semicolon)) {
            if (auto expected = self.next_token(); not expected) {
                return error_t{ std::move(expected).error() };
            }

            auto expected_expression = self.parse_statement(0);

            if (not expected_expression) {
                return expected_expression;
            }

            ret.value = std::move(expected_expression).value();
        }
        else ret.value = parse_null(self).value();

        return new_node;
    }

}
