#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_identifier(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = ast::identifier{};
        auto &ident = std::get<ast::identifier>(new_node->data);
        ident.name = std::get<types::string_t>(self.current_token.token);
        return new_node;
    }

}
