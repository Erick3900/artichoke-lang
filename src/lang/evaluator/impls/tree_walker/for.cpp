#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_for_loop(const ast::for_loop &for_loop, const std::shared_ptr<scope> &env) {
        auto new_env = scope::new_enclosed(env);

        if (for_loop.before) {
            auto expected_before = eval(for_loop.before.get(), new_env);

            if (not expected_before) {
                return expected_before;
            }
        }

        while (true) {
            if (for_loop.condition) {
                auto expected_condition = eval(for_loop.condition.get(), new_env);

                if (not expected_condition) {
                    return expected_condition;
                }

                if (not expected_condition.value().is_truthy()) {
                    break;
                }
            }

            auto expected_body = eval(for_loop.body.get(), new_env);

            if (not expected_body) {
                return expected_body;
            }

            if (for_loop.after) {
                auto expected_after = eval(for_loop.after.get(), new_env);

                if (not expected_after) {
                    return expected_after;
                }
            }
        }

        return types::null_t{};
	}

}
