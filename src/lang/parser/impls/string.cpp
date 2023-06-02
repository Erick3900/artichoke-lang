#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_string(parser &self) {
        auto new_node = ast::new_node();
        auto &str = std::get<types::string_t>(self.current_token.token);
        new_node->data = str.substr(1, str.size() - 2);
        return new_node;
    }

}
