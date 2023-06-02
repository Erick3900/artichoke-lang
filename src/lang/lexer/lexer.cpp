#include "lexer/lexer.hpp"

#include <fmt/format.h>

#include "utils/strings.hpp"

namespace arti::lang {

    lexer::lexer(std::string source_code)
        : line(0)
        , column(0)
        , source_code(source_code)
        , tokens_iterator()
        , tokens_generator() {
        tokens_generator = init();
        build_op_trie();
    }

    void lexer::forward() {
        if (tokens_iterator.handle == nullptr) {
            tokens_iterator = begin();
        }
        else {
            if (not tokens_iterator.handle.done())
                ++tokens_iterator;
        }
    }

    lexer::value_type lexer::next_token() {
        if (tokens_iterator.handle.done()) {
            return error_t{ "EOF reached" };
        }
        forward();
        return *tokens_iterator;
    }

    lexer::value_type lexer::current_token() {
        if (tokens_iterator.handle == nullptr) {
            tokens_iterator = begin();
        }

        return *tokens_iterator;
    }

    lexer::generator &lexer::getGenerator() {
        return tokens_generator;
    }

    lexer::iterator lexer::begin() {
        return tokens_generator.begin();
    }

    lexer::iterator_sentinel lexer::end() {
        return tokens_generator.end();
    }

    lexer::generator lexer::init() {
        token ctok;

        auto iter = source_code.begin();

        while (iter != source_code.end()) {
            iter = skip_whitespace(iter);

            if (iter == source_code.end()) {
                break;
            }

            auto biter = iter;

            if (str::is_digit(*iter)) {
                auto expected_number = read_number(iter);

                if (not expected_number) {
                    co_yield error_t{ std::move(expected_number).error() };
                    co_return;
                }

                auto [value, is_decimal] = std::move(expected_number).value();

                if (is_decimal) {
                    ctok = new_decimal(std::move(value));
                }
                else {
                    ctok = new_integer(std::move(value));
                }
            }
            else if (*iter == '"') {
                auto expected_string = read_string(iter);

                if (not expected_string) {
                    co_yield error_t{ std::move(expected_string).error() };
                    co_return;
                }

                ctok = new_string(std::move(expected_string).value());
            }
            else if (*iter == '\'') {
                auto expected_char = read_char(iter);

                if (not expected_char) {
                    co_yield error_t{ std::move(expected_char).error() };
                    co_return;
                }

                ctok = new_char(std::move(expected_char).value());
            }
            else if (*iter == ':') { ctok = new_simple(token_type::colon); }
            else if (*iter == ',') { ctok = new_simple(token_type::comma); }
            else if (*iter == '(') { ctok = new_simple(token_type::lparen); }
            else if (*iter == ')') { ctok = new_simple(token_type::rparen); }
            else if (*iter == '[') { ctok = new_simple(token_type::lbracket); }
            else if (*iter == ']') { ctok = new_simple(token_type::rbracket); }
            else if (*iter == '{') { ctok = new_simple(token_type::lsquirly); }
            else if (*iter == '}') { ctok = new_simple(token_type::rsquirly); }
            else if (*iter == ';') { ctok = new_simple(token_type::semicolon); }
            else { ctok = match(iter); }

            ctok.line = line;
            ctok.column = column;

            ++iter;

            column += (iter - biter);
            co_yield ctok;
        }

        ctok = new_eof();
        co_yield ctok;
    }

    std::string::iterator lexer::skip_whitespace(std::string::iterator it) {
        while (it != source_code.end()) {
            if (*it == ' ') {
                ++it;
                ++column;
            }
            else  if (*it == '\t') {
                ++it;
                column += 4;
            }
            else if (*it == '\n') {
                ++it;
                ++line;
                column = 0;
            }
            else break;
        }

        return it;
    }

    expected_t<std::pair<std::string, bool>> lexer::read_number(std::string::iterator &iter) {
        auto begin = iter;
        bool has_dot = false;

        while (iter != source_code.end() && (str::is_digit(*iter) or *iter == '.')) {
            if (*iter == '.') {
                if (has_dot) {
                    return error_t{ "Invalid number literal" };
                }
                else {
                    has_dot = true;
                }
            }
            ++iter;
        }

        return std::pair{ std::string{ begin, iter-- }, has_dot };
    }

    expected_t<std::string> lexer::read_char(std::string::iterator &iter) {
        std::string str;

        ++iter;

        if (*iter == '\\') {
            switch(*++iter) {
                case '"': str.push_back('"'); break;
                case 't': str.push_back('\t'); break;
                case 'b': str.push_back('\b'); break;
                case 'n': str.push_back('\n'); break;
                case 'r': str.push_back('\r'); break;
                case 'f': str.push_back('\f'); break;
                case '\'': str.push_back('\''); break;
                case '\\': str.push_back('\\'); break;
                default:
                    return error_t{ "Invalid scape char" };
            }
        }
        else str.push_back(*iter);

        if (*++iter != '\'') {
            return error_t{ "Invalid char literal" };
        }

        return str;
    }

    expected_t<std::string> lexer::read_string(std::string::iterator &iter) {
        std::string str;

        ++iter;

        while (iter != source_code.end() && *iter != '"') {
            if (*iter == '\\') {
                switch(*++iter) {
                    case '"': str.push_back('"'); break;
                    case 't': str.push_back('\t'); break;
                    case 'b': str.push_back('\b'); break;
                    case 'n': str.push_back('\n'); break;
                    case 'r': str.push_back('\r'); break;
                    case 'f': str.push_back('\f'); break;
                    case '\'': str.push_back('\''); break;
                    case '\\': str.push_back('\\'); break;
                    default:
                        return error_t{ "Invalid scape char" };
                }
            }
            else {
                str.push_back(*iter);
            }

            ++iter;
        }

        return "\"" + str + "\"";
    }

    void lexer::build_op_trie() {
        for (const auto &[v, k] : keywords_strs) {
            std::string key{ k };
            keywords[key] = v;
        }

        for (const auto &[v, k] : tokens) {
            ops.insert(k, v);
        }
    }

    token lexer::new_eof() const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::eof,
            .token = ""
        };
    }

    token lexer::new_char(std::string_view value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::character,
            .token = std::string{ value }
        };
	}

    token lexer::new_string(std::string_view value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::string,
            .token = std::string{ value }
        };
	}

    token lexer::new_decimal(std::string_view value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::decimal,
            .token = std::string{ value }
        };
    }

    token lexer::new_integer(std::string_view value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::integer,
            .token = std::string{ value }
        };
	}

    token lexer::new_identifier(std::string_view value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::identifier,
            .token = std::string{ value }
        };
	}

    token lexer::new_fallthrough(std::string value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::fallthrough,
            .token = std::string{ value }
        };
    }

    token lexer::new_simple(token_type type) const {
        return token{
            .line = line,
            .column = column,
            .type = type,
            .token = ""
        };
    }

    token lexer::new_op(token_value value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::op,
            .token = value
        };
    }

    token lexer::new_keyword(token_value value) const {
        return token{
            .line = line,
            .column = column,
            .type = token_type::keyword,
            .token = value
        };
    }

    void lexer::op_trie::insert(std::string_view str, token_value op) {
        auto it = str.begin();
        auto node_ptr = &root;

        while (it != str.end()) {
            node_ptr = &node_ptr->childs[*it++];
        }

        node_ptr->op = op;
        node_ptr->is_end = true;
    }

    token lexer::lookup_identifier(std::string value) const {
        if (auto it = keywords.find(value); it != keywords.end()) {
            return new_keyword(it->second);
        }
        return new_identifier(value);
    }

    token lexer::match(std::string::iterator &iter) {
        auto biter = iter;

        if (ops.root.childs.contains(*iter)) {
            auto node_ptr = &ops.root.childs.at(*iter);
            std::size_t last = node_ptr->is_end ? 0 : 1;

            ++iter;

            while (iter != source_code.end() && not str::is_whitespace(*iter)) {
                if (node_ptr->childs.contains(*iter)) {
                    node_ptr = &node_ptr->childs.at(*iter);
                }
                else {
                    if (last == 0) { --iter; return new_op(node_ptr->op); }
                    else { --iter; return new_fallthrough({ biter, iter + 2 }); }
                }

                ++iter;
                ++last;

                if (node_ptr->is_end) {
                    last = 0;
                }
            }

            if (last == 0) { --iter; return new_op(node_ptr->op); }
            else { return new_fallthrough({ biter, iter-- }); }
        }
        else if (str::is_letter(*iter)) {
            while (iter != source_code.end() && str::is_letter(*iter)) {
                ++iter;
            }

            return lookup_identifier({ biter, iter-- });
        }

        while (iter != source_code.end() && not str::is_whitespace(*iter) && not str::is_letter(*iter)) {
            ++iter;
        }

        return new_fallthrough({ biter, iter-- });
    }

    std::string_view token_type_to_string(token_type type) {
        switch(type) {
            case token_type::eof: return "eof";
            case token_type::op: return "op";
            case token_type::string: return "string";
            case token_type::keyword: return "keyword";
            case token_type::integer: return "integer";
            case token_type::decimal: return "decimal";
            case token_type::character: return "character";
            case token_type::identifier: return "identifier";
            case token_type::lparen: return "lparen";
            case token_type::rparen: return "rparen";
            case token_type::lbracket: return "lbracket";
            case token_type::rbracket: return "rbracket";
            case token_type::lsquirly: return "lsquirly";
            case token_type::rsquirly: return "rsquirly";
            case token_type::semicolon: return "semicolon";
            case token_type::colon: return "colon";
            case token_type::comma: return "comma";
            case token_type::fallthrough: return "fallthrough";
        }
        return "<unknown>";
    }

    std::string_view token_value_to_string(const std::variant<token_value, std::string> &token) {
        if (std::holds_alternative<token_value>(token)) {
            switch(std::get<token_value>(token)) {
                case token_value::dot: return "dot";
                case token_value::mod: return "mod";
                case token_value::plus: return "plus";
                case token_value::minus: return "minus";
                case token_value::slash: return "slash";
                case token_value::bang: return "bang";
                case token_value::asterisk: return "asterisk";
                case token_value::assign: return "assign";
                case token_value::less_than: return "less_than";
                case token_value::greater_than: return "greater_than";
                case token_value::left_shift: return "left_shift";
                case token_value::right_shift: return "right_shift";
                case token_value::equal: return "equal";
                case token_value::not_equal: return "not_equal";
                case token_value::greater_than_equal: return "greater_than_equal";
                case token_value::less_than_equal: return "less_than_equal";
                case token_value::bool_and: return "bool_and";
                case token_value::bool_or: return "bool_or";
                case token_value::bit_and: return "bit_and";
                case token_value::bit_or: return "bit_or";
                case token_value::bit_xor: return "bit_xor";
                case token_value::bit_not: return "bit_not";
                case token_value::namespace_access: return "namespace_access";
                case token_value::k_if: return "k_if";
                case token_value::k_or: return "k_or";
                case token_value::k_fn: return "k_fn";
                case token_value::k_not: return "k_not";
                case token_value::k_for: return "k_for";
                case token_value::k_and: return "k_and";
                case token_value::k_let: return "k_let";
                case token_value::k_else: return "k_else";
                case token_value::k_true: return "k_true";
                case token_value::k_null: return "k_null";
                case token_value::k_const: return "k_const";
                case token_value::k_while: return "k_while";
                case token_value::k_false: return "k_false";
                case token_value::k_return: return "k_return";
            }
        }

        return std::get<std::string>(token);
    }

    std::string token::to_string() const {
        return fmt::format(
            "{}: '{}', line {}, column {}",
            token_type_to_string(type),
            token_value_to_string(token),
            line + 1,
            column + 1
        );
    }

}
