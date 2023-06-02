#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_while_loop(const ast::while_loop &while_loop, const std::shared_ptr<scope> &env) {
        auto new_env = scope::new_enclosed(env);

        while (true) {
            auto expected_condition = eval(while_loop.condition.get(), new_env);

            if (not expected_condition) {
                return expected_condition;
            }

            if (not expected_condition.value().is_truthy()) {
                break;
            }

            auto expected_body = eval(while_loop.body.get(), new_env);

            if (not expected_body) {
                return expected_body;
            }
        }

        return types::null_t{};
    }

}
