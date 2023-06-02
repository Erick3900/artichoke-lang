#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_boolean(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = std::get<token_value>(self.current_token.token) == token_value::k_true;
        return new_node;
    }

}
