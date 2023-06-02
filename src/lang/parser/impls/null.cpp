#include "parser/parser.hpp"

namespace arti::lang {

    expected_t<ast::ptr> parse_null(parser &self) {
        auto new_node = ast::new_node();
        new_node->data = types::null_t{};
        return new_node;
    }

}
