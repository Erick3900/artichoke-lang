#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_return(const ast::return_value &return_expr, const std::shared_ptr<scope> &env) {
        auto expected_condition = eval(return_expr.value.get(), env);

        if (not expected_condition) {
            return expected_condition;
        }

        // if (expected_condition->is_reference() and expected_condition->unwrap().is_object()) {
        //     if (not expected_condition->unwrap().is<types::fn_t>()) {
        //         expected_condition = expected_condition->unwrap();
        //     }
        // }

        expected_condition->is_return = true;

        return std::move(expected_condition).value();
	}

}
