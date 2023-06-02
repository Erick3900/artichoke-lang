#include "parser/parser.hpp"

#include <fmt/format.h>

namespace arti::lang {

    expected_t<ast::ptr> parse_unary(parser &self) {
        auto new_node = ast::new_node();

        new_node->data = ast::unary{};

        auto &unary = std::get<ast::unary>(new_node->data);
        auto value = std::get<token_value>(self.current_token.token);

        switch (value) {
            case token_value::bang:
                unary.op = ast::operators::boolean_not;
                break;
            case token_value::k_not:
                unary.op = ast::operators::boolean_not;
                break;
            case token_value::bit_not:
                unary.op = ast::operators::bit_not;
                break;
            case token_value::minus:
                unary.op = ast::operators::substraction;
                break;
            default:
                return error_t{ fmt::format("Unknown unary operator '{}'", token_value_to_string(value)) };
        }

        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_expr = self.parse_statement(self.precedence(self.current_token, true));

        if (not expected_expr) {
            return expected_expr;
        }

        unary.right = std::move(expected_expr).value();

        return new_node;
    }

}
