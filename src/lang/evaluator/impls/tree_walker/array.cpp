#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_array(const ast::array &arr, const std::shared_ptr<scope> &env) {
        auto obj = types::object_t{};
        obj.value = types::array_t{};

        auto &arr_ref = obj.as<types::array_t>();

        for (const auto &expr : arr.elements) {
            auto expected_expr = eval(expr.get(), env);

            if (not expected_expr) {
                return expected_expr;
            }

            arr_ref.emplace_back(std::move(expected_expr).value());
        }

        return obj;
    }

}
