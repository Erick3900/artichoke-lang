#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_if_else(const ast::if_else &if_else, const std::shared_ptr<scope> &env) {
        auto expected_condition = eval(if_else.condition.get(), env);

        if (not expected_condition) {
            return expected_condition;
        }

        auto new_env = scope::new_enclosed(env);

        if (expected_condition.value().is_truthy()) {
            return eval(if_else.if_branch.get(), new_env);
        }
        else if (if_else.else_branch) {
            return eval(if_else.else_branch.get(), new_env);
        }

        return types::null_t{};
	}

}
