#include "parser/parser.hpp"

#include <fmt/format.h>

namespace arti::lang {

    expected_t<ast::ptr> parse_decimal(parser &self) {
        auto new_node = ast::new_node();
        auto &value = std::get<types::string_t>(self.current_token.token);
        try {
            new_node->data = std::stod(value);
        }
        catch (std::exception &) {
            return error_t{ fmt::format("Coudln't parse '{}' as a decimal value", value) };
        }
        return new_node;
    }

}
