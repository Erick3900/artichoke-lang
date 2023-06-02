#include "parser/parser.hpp"

#include <fmt/format.h>

namespace arti::lang {

    expected_t<ast::ptr> parse_binary(parser &self, ast::ptr left) {
        auto new_node = ast::new_node();

        new_node->data = ast::binary{};

        auto &binary = std::get<ast::binary>(new_node->data);

        binary.left = std::move(left);

        auto value = std::get<token_value>(self.current_token.token);

        switch (value) {
            case token_value::k_and:
                binary.op = ast::operators::boolean_and;
                break;
            case token_value::k_or:
                binary.op = ast::operators::boolean_or;
                break;
            case token_value::bool_and:
				binary.op = ast::operators::boolean_and;
				break;
            case token_value::bool_or:
				binary.op = ast::operators::boolean_or;
				break;
            case token_value::mod:
				binary.op = ast::operators::modulo;
				break;
            case token_value::plus:
				binary.op = ast::operators::addition;
				break;
            case token_value::minus:
				binary.op = ast::operators::substraction;
				break;
            case token_value::slash:
				binary.op = ast::operators::division;
				break;
            case token_value::equal:
				binary.op = ast::operators::equal;
				break;
            case token_value::bit_or:
				binary.op = ast::operators::bit_or;
				break;
            case token_value::bit_and:
				binary.op = ast::operators::bit_and;
				break;
            case token_value::bit_xor:
				binary.op = ast::operators::bit_xor;
				break;
            case token_value::asterisk:
				binary.op = ast::operators::multiplication;
				break;
            case token_value::less_than:
				binary.op = ast::operators::less_than;
				break;
            case token_value::not_equal:
				binary.op = ast::operators::not_equal;
				break;
            case token_value::left_shift:
				binary.op = ast::operators::bit_left_shift;
				break;
            case token_value::right_shift:
				binary.op = ast::operators::bit_right_shift;
				break;
            case token_value::greater_than:
				binary.op = ast::operators::greater_than;
				break;
            case token_value::less_than_equal:
				binary.op = ast::operators::less_than_equal;
				break;
            case token_value::greater_than_equal:
				binary.op = ast::operators::greater_than_equal;
				break;
            default:
                return error_t{ fmt::format("Unknown binary operator '{}'", token_value_to_string(value)) };
        }

        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_expr = self.parse_statement(self.precedence(self.current_token, true));

        if (not expected_expr) {
            return expected_expr;
        }

        binary.right = std::move(expected_expr).value();

        return new_node;
    }

}
