#pragma once

#include "parser/ast.hpp"

namespace arti::lang {

    namespace concepts {
        template <typename T>
        concept evaluator = requires(T t) {
            { t.eval(std::declval<ast_node *>()) } -> std::same_as<expected_t<types::object_t>>;
            { t.eval(std::declval<ast_node *>(), std::declval<const std::shared_ptr<scope> &>()) } -> std::same_as<expected_t<types::object_t>>;
            requires std::is_same_v<decltype(t.builtins), std::unordered_map<std::string_view, types::native_fn_t>>;
        };
    };

}
