#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_character(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = types::char_t{ std::get<types::string_t>(self.current_token.token).at(0) };
        return new_node;
    }

}
