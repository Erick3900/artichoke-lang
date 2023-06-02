#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_assign(const ast::assign &assign, const std::shared_ptr<scope> &env) {
        auto var = assign.variable.get();

        if (std::holds_alternative<ast::identifier>(var->data)) {
            auto &ident = std::get<ast::identifier>(var->data);

            if (not env->is_defined(ident.name)) {
                return error_t{ fmt::format("Identifier '{}' does not exists", ident.name) };
            }

            auto &ref = env->get_ref(ident.name).value().get();

            if (ref.is_const) {
                return error_t{ "Cannot reassign a const variable" };
            }

            auto expected_value = eval(assign.value.get(), env);

            if (not expected_value) {
                return expected_value;
            }

            ref = std::move(expected_value).value();

            return types::null_t{};
        }
        else {
            auto expected_ref = eval(var, env);

            if (not expected_ref) {
                return expected_ref;
            }

            if (not (expected_ref->is_reference())) {
                return error_t{ "Expected left side of index expression to be a reference" };
            }

            auto &ref = expected_ref->unwrap();

            if (ref.is_const) {
                return error_t{ "Cannot reassign a const variable" };
            }

            auto expected_value = eval(assign.value.get(), env);

            if (not expected_value) {
                return expected_value;
            }

            ref = std::move(expected_value).value();

            return types::null_t{};
        }
    }

}
