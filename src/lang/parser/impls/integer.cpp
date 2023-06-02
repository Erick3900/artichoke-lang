#include "parser/parser.hpp"

#include <fmt/format.h>

namespace arti::lang {

    expected_t<ast::ptr> parse_integer(parser &self) {
        auto new_node = ast::new_node();
        auto &value = std::get<types::string_t>(self.current_token.token);
        try {
            new_node->data = std::stoll(value);
        }
        catch(std::exception &) {
            return error_t{ fmt::format("Couldn't parse '{}' as an integer value", value) };
        }
        return new_node;
    }

}
