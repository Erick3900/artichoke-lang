#pragma once

#include <array>
#include <tuple>
#include <variant>
#include <string_view>

namespace arti::lang {

    enum class token_type {
        eof,
        op,
        string,
        keyword,
        integer,
        decimal,
        character,
        identifier,
        lparen,
        rparen,
        lbracket,
        rbracket,
        lsquirly,
        rsquirly,
        semicolon,
        colon,
        comma,
        fallthrough
    };

    enum class token_value {
        dot,
        mod,
        plus,
        minus,
        slash,
        bang,
        asterisk,
        assign,
        less_than,
        greater_than,
        left_shift,
        right_shift,
        equal,
        not_equal,
        greater_than_equal,
        less_than_equal,
        bool_and,
        bool_or,
        bit_and,
        bit_or,
        bit_xor,
        bit_not,
        namespace_access,
        k_if,
        k_or,
        k_fn,
        k_not,
        k_for,
        k_and,
        k_let,
        k_else,
        k_true,
        k_null,
        k_const,
        k_while,
        k_false,
        k_return
    };

    inline constexpr std::array<std::tuple<token_value, std::string_view>, 14> keywords_strs{
        std::tuple<token_value, std::string_view>{    token_value::k_if,     "if"},
        std::tuple<token_value, std::string_view>{    token_value::k_or,     "or"},
        std::tuple<token_value, std::string_view>{    token_value::k_fn,     "fn"},
        std::tuple<token_value, std::string_view>{   token_value::k_not,    "not"},
        std::tuple<token_value, std::string_view>{   token_value::k_for,    "for"},
        std::tuple<token_value, std::string_view>{   token_value::k_and,    "and"},
        std::tuple<token_value, std::string_view>{   token_value::k_let,    "let"},
        std::tuple<token_value, std::string_view>{  token_value::k_else,   "else"},
        std::tuple<token_value, std::string_view>{  token_value::k_true,   "true"},
        std::tuple<token_value, std::string_view>{  token_value::k_null,   "null"},
        std::tuple<token_value, std::string_view>{ token_value::k_const,  "const"},
        std::tuple<token_value, std::string_view>{ token_value::k_while,  "while"},
        std::tuple<token_value, std::string_view>{ token_value::k_false,  "false"},
        std::tuple<token_value, std::string_view>{token_value::k_return, "return"},
    };

    inline constexpr std::array<std::tuple<token_value, std::string_view>, 23> tokens{
        std::tuple<token_value, std::string_view>{               token_value::dot,  "."},
        std::tuple<token_value, std::string_view>{               token_value::mod,  "%"},
        std::tuple<token_value, std::string_view>{              token_value::plus,  "+"},
        std::tuple<token_value, std::string_view>{             token_value::minus,  "-"},
        std::tuple<token_value, std::string_view>{             token_value::slash,  "/"},
        std::tuple<token_value, std::string_view>{              token_value::bang,  "!"},
        std::tuple<token_value, std::string_view>{          token_value::asterisk,  "*"},
        std::tuple<token_value, std::string_view>{            token_value::assign,  "="},
        std::tuple<token_value, std::string_view>{         token_value::less_than,  "<"},
        std::tuple<token_value, std::string_view>{      token_value::greater_than,  ">"},
        std::tuple<token_value, std::string_view>{        token_value::left_shift, "<<"},
        std::tuple<token_value, std::string_view>{       token_value::right_shift, ">>"},
        std::tuple<token_value, std::string_view>{             token_value::equal, "=="},
        std::tuple<token_value, std::string_view>{         token_value::not_equal, "!="},
        std::tuple<token_value, std::string_view>{token_value::greater_than_equal, ">="},
        std::tuple<token_value, std::string_view>{   token_value::less_than_equal, "<="},
        std::tuple<token_value, std::string_view>{          token_value::bool_and, "&&"},
        std::tuple<token_value, std::string_view>{           token_value::bool_or, "||"},
        std::tuple<token_value, std::string_view>{           token_value::bit_and,  "&"},
        std::tuple<token_value, std::string_view>{            token_value::bit_or,  "|"},
        std::tuple<token_value, std::string_view>{           token_value::bit_xor,  "^"},
        std::tuple<token_value, std::string_view>{           token_value::bit_not,  "~"},
        std::tuple<token_value, std::string_view>{  token_value::namespace_access, "::"},
    };

    std::string_view token_type_to_string(token_type type);
    std::string_view token_value_to_string(const std::variant<token_value, std::string> &token);

}    // namespace arti::lang
