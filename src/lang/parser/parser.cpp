#include "parser/parser.hpp"

#include <fmt/format.h>

namespace arti::lang {

    parser::parser(std::string source_code)
        : lxr{ nullptr } {
        lxr = std::make_unique<lexer>(std::move(source_code));
        auto _ = next_token();
    }

    expected_t<void> parser::next_token() {
        auto expect_current = lxr->current_token();

        if (not expect_current) {
            return error_t{ std::move(expect_current).error() };
        }

        auto expect_next = lxr->next_token();

        if (not expect_next) {
            return error_t{ std::move(expect_next).error() };
        }

        current_token = std::move(expect_current).value();
        peek_token = std::move(expect_next).value();

        return {};
    }

    expected_t<parser::ast_ptr> parser::parse() {
        return parse_statements_until(token_type::eof);
    }

    expected_t<parser::unary_parser_t> parser::get_unary_callback(const token &tok) {
        switch (tok.type) {
            case token_type::string:
                return parse_string;
            case token_type::integer:
                return parse_integer;
            case token_type::decimal:
                return parse_decimal;
            case token_type::character:
                return parse_character;
            case token_type::lsquirly:
                return parse_hash;
            case token_type::lbracket:
                return parse_array;
            case token_type::lparen:
                return parse_grouped;
            case token_type::identifier:
                return parse_identifier;
            case token_type::op: {
                auto value = std::get<token_value>(tok.token);
                if (value == token_value::bit_not || value == token_value::minus || value == token_value::bang) {
                    return parse_unary;
                }
                else {
                    return error_t{ "No parser available" };
                }
            }
            case token_type::keyword: {
                auto value = std::get<token_value>(tok.token);
                switch (value) {
                    case token_value::k_null:
                        return parse_null;
                    case token_value::k_if:
                        return parse_if;
                    case token_value::k_fn:
                        return parse_fn;
                    case token_value::k_for:
                        return parse_for;
                    case token_value::k_true:
                        return parse_boolean;
                    case token_value::k_false:
                        return parse_boolean;
                    case token_value::k_let:
                        return parse_declaration;
                    case token_value::k_const:
                        return parse_declaration;
                    case token_value::k_return:
                        return parse_return;
                    case token_value::k_while:
                        return parse_while;
                    case token_value::k_not:
                        return parse_unary;
                    default:
                        return error_t{ "No parser available" };
                }
            }
            default:
                return error_t{ "No parser available" };
        }
    }

    expected_t<parser::binary_parser_t> parser::get_binary_callback(const token &tok) {
        switch (tok.type) {
            case token_type::lbracket:
                return parse_index;
            case token_type::lparen:
                return parse_call;
            case token_type::op: {
                auto value = std::get<token_value>(tok.token);
                if (value == token_value::bool_and || value == token_value::bool_or || value == token_value::mod ||
                    value == token_value::plus || value == token_value::minus || value == token_value::slash ||
                    value == token_value::equal || value == token_value::bit_or || value == token_value::bit_and ||
                    value == token_value::bit_xor || value == token_value::asterisk ||
                    value == token_value::less_than || value == token_value::not_equal ||
                    value == token_value::left_shift || value == token_value::right_shift ||
                    value == token_value::greater_than || value == token_value::less_than_equal ||
                    value == token_value::greater_than_equal) {
                    return parse_binary;
                }
                else if (value == token_value::assign) {
                    return parse_assign;
                }
                else {
                    return error_t{ "No parser available" };
                }
            }
            case token_type::keyword: {
                auto value = std::get<token_value>(tok.token);
                if (value == token_value::k_and or value == token_value::k_or) {
                    return parse_binary;
                }
                else {
                    return error_t{ "No parser available" };
                }
            }
            default:
                return error_t{ "No parser available" };
        }
    }

    expected_t<parser::ast_ptr> parser::parse_statement(std::size_t precedence) {
        auto unary_callback = get_unary_callback(current_token);

        if (not unary_callback) {
            return error_t{ fmt::format("No parser available for: {}", current_token.to_string()) };
        }

        auto expected_expression = (*unary_callback)(*this);

        if (not expected_expression) {
            return expected_expression;
        }

        auto left_expression = std::move(expected_expression).value();

        while (not(peek_token.type == token_type::semicolon) and (precedence < peek_precedence())) {
            auto binary_callback = get_binary_callback(peek_token);

            if (not binary_callback) {
                return left_expression;
            }

            if (auto expected = next_token(); not expected) {
                return error_t{ std::move(expected).error() };
            }

            expected_expression = (*binary_callback)(*this, std::move(left_expression));

            if (not expected_expression) {
                return expected_expression;
            }

            left_expression = std::move(expected_expression).value();
        }

        return left_expression;
    }

    expected_t<parser::ast_ptr> parser::parse_statements_until(token_type type) {
        auto node = ast::new_node();
        node->data = std::list<ast::ptr>{};
        auto &list = std::get<std::list<ast::ptr>>(node->data);

        while (current_token.type != type and current_token.type != token_type::eof) {
            auto expected_statement = parse_statement(0);

            if (not expected_statement) {
                return expected_statement;
            }

            if (not peek_expect(token_type::eof)) {
                if (auto ok = next_token(); not ok) {
                    return error_t{ std::move(ok).error() };
                }
            }
            else {
                current_token = peek_token;
            }

            if (auto expected = expect_and_next(token_type::semicolon); expected) {
                expected_statement->get()->had_semicolon = true;
            }

            list.push_back(std::move(expected_statement).value());
        }

        return node;
    }

    std::size_t parser::current_precedence() {
        return precedence(current_token);
    }

    std::size_t parser::peek_precedence() {
        return precedence(peek_token);
    }

    std::size_t parser::precedence(const token &tok, bool unary) {
        if (unary) {
            return 24;
        }

        switch (tok.type) {
            case token_type::keyword:
            case token_type::op: {
                auto value = std::get<token_value>(tok.token);
                switch (value) {
                    case token_value::assign:               return 2;
                    case token_value::k_or:                 return 4;
                    case token_value::bool_or:              return 4;
                    case token_value::k_and:                return 6;
                    case token_value::bool_and:             return 6;
                    case token_value::bit_or:               return 8;
                    case token_value::bit_xor:              return 10;
                    case token_value::bit_and:              return 12;
                    case token_value::equal:                return 14;
                    case token_value::not_equal:            return 14;
                    case token_value::less_than:            return 16;
                    case token_value::greater_than:         return 16;
                    case token_value::less_than_equal:      return 16;
                    case token_value::greater_than_equal:   return 16;
                    case token_value::left_shift:           return 18;
                    case token_value::right_shift:          return 18;
                    case token_value::plus:                 return 20;
                    case token_value::minus:                return 20;
                    case token_value::slash:                return 22;
                    case token_value::asterisk:             return 22;
                    case token_value::mod:                  return 22;
                    default: return 0;
                }
            }
            case token_type::lparen:    return 26;
            case token_type::lbracket:  return 28;
            default: return 0;
        }
        return 0;
    }

    expected_t<void> parser::expect(token_type type) {
        if (current_token.type != type) {
            return error_t{ fmt::format(
                "Expected '{}' at line {} column {}, got '{}'",
                token_type_to_string(type),
                current_token.line + 1,
                current_token.column + 1,
                token_type_to_string(current_token.type)
            ) };
        }
        return {};
    }

    expected_t<void> parser::peek_expect(token_type type) {
        if (peek_token.type != type) {
            return error_t{ fmt::format(
                "Expected '{}' at line {} column {}, got '{}'",
                token_type_to_string(type),
                peek_token.line + 1,
                peek_token.column + 1,
                token_type_to_string(peek_token.type)
            ) };
        }
        return {};
    }

    expected_t<void> parser::expect_and_next(token_type type) {
        if (auto expected = expect(type); not expected) {
            return expected;
        }

        if (auto ok = next_token(); not ok) {
            return ok;
        }

        return {};
    }

    expected_t<void> parser::peek_expect_and_next(token_type type) {
        if (auto expected = peek_expect(type); not expected) {
            return expected;
        }

        if (auto ok = next_token(); not ok) {
            fmt::print("{}\n", ok.error());
            return ok;
        }

        return {};
    }

}    // namespace arti::lang
