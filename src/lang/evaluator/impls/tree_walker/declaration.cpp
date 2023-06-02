#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_declaration(const ast::declaration &decl, const std::shared_ptr<scope> &env) {
        if (env->is_defined(decl.variable)) {
            return error_t{ fmt::format("Variable '{}' is already defined", decl.variable) };
        }

        auto expected_value = eval(decl.value.get(), env);

        if (not expected_value) {
            return expected_value;
        }

        expected_value->is_const = decl.is_const;

        if (expected_value->is_reference() and not expected_value->unwrap().is_object()) {
            env->set(decl.variable, expected_value->unwrap());
        }
        else {
            env->set(decl.variable, std::move(expected_value).value());
        }

        return types::null_t{};
	}

}
