#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_declaration(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::declaration{};
        auto &decl = std::get<ast::declaration>(new_node->data);

        decl.is_const = std::get<token_value>(self.current_token.token) == token_value::k_const;

        if (auto expected = self.peek_expect_and_next(token_type::identifier); not expected) {
            return error_t{ std::move(expected).error() };
        }

        decl.variable = std::get<types::string_t>(self.current_token.token);

        if (not self.peek_expect(token_type::semicolon)) {
            if (auto expected = self.peek_expect_and_next(token_type::op); not expected) {
                return error_t{ std::move(expected).error() };
            }

            if (std::get<token_value>(self.current_token.token) != token_value::assign) {
                return error_t{ "Expected assignment for the declaration" };
            }

            if (auto expected = self.next_token(); not expected) {
                return error_t{ std::move(expected).error() };
            }

            auto expected_expression = self.parse_statement(0);

            if (not expected_expression) {
                return expected_expression;
            }

            decl.value = std::move(expected_expression).value();
        }
        else decl.value = parse_null(self).value();

        return new_node;
    }

}
