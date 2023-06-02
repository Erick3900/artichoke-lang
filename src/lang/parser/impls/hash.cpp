#include "parser/parser.hpp"

namespace arti::lang {
    expected_t<ast::hash_pair> parse_hash_pair(parser &self);

    expected_t<ast::ptr> parse_hash(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::hash{};
        auto &hash = std::get<ast::hash>(new_node->data);

        if (not self.peek_expect_and_next(token_type::rsquirly)) {
            do {
                if (auto expected = self.next_token(); not expected) {
                    return error_t{ std::move(expected).error() };
                }

                auto expected_expression = parse_hash_pair(self);

                if (not expected_expression) {
                    return error_t{ std::move(expected_expression).error() };
                }

                hash.elements.emplace_back(std::move(expected_expression).value());
            } while (self.peek_expect_and_next(token_type::comma));

            if (auto expected = self.peek_expect_and_next(token_type::rsquirly); not expected) {
                return error_t{ std::move(expected).error() };
            }
        }

        return new_node;
    }

    expected_t<ast::hash_pair> parse_hash_pair(parser &self) {
        ast::hash_pair hp;

        if (self.expect_and_next(token_type::lbracket)) {
                auto expected_key = self.parse_statement(0);

                if (not expected_key) {
                    return error_t{ std::move(expected_key).error() };
                }

                hp.key = std::move(expected_key).value();

                if (auto expected = self.peek_expect_and_next(token_type::rbracket); not expected) {
                    return error_t{ std::move(expected).error() };
                }
        }
        else {
            if (auto ok = self.expect(token_type::identifier); not ok) {
                return error_t{ std::move(ok).error() };
            }

            auto n_key = ast::new_node();
            n_key->data = std::get<types::string_t>(self.current_token.token);
            hp.key = std::move(n_key);

        }

        if (auto expected = self.peek_expect_and_next(token_type::colon); not expected) {
            return error_t{ std::move(expected).error() };
        }

        if (auto expected = self.next_token(); not expected) {
            return error_t{ std::move(expected).error() };
        }

        auto expected_value = self.parse_statement(0);

        if (not expected_value) {
            return error_t{ std::move(expected_value).error() };
        }

        hp.value = std::move(expected_value).value();

        return hp;
    }

}
