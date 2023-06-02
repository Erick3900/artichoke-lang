#pragma once

namespace arti::str {

    static constexpr bool is_letter(char c) {
        return (
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_'
        );
    }

    static constexpr bool is_digit(char c) {
        return (
            c >= '0' && c <= '9'
        );
    }

    static constexpr bool is_whitespace(char c) {
        return (
            c == ' ' ||
            c == '\t' ||
            c == '\n'
        );
    }

}
