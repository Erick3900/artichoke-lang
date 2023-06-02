#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::resolve(const ast::identifier &ident, const std::shared_ptr<scope> &env) {
        auto ident_value = env->get(ident.name);

        if (not ident_value) {
            if (auto it = builtins.find(ident.name); it != builtins.end()) {
                return types::object_t{ it->second };
            }
            return error_t{ std::move(ident_value).error() };
        }

        return std::move(ident_value).value();
    }

}
