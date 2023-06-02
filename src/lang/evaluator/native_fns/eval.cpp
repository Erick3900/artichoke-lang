#include "evaluator/native_fns.hpp"

#include "evaluator/object.hpp"
#include "evaluator/scope.hpp"

namespace arti::lang::native_fns {

    expected_t<types::object_t> eval(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() != 1) {
            return error_t{ "Number of arguments mismatch" };
        }

        auto &str_obj = params.front();

        if (not str_obj.is<types::string_t>()) {
            return error_t{ "Eval expected a source string" };
        }

        auto &unw_src = str_obj.as<types::string_t>();

        auto prsr = parser{ unw_src };
        auto expected_ast = prsr.parse();

        if (not expected_ast) {
            return error_t{ std::move(expected_ast).error() };
        }

        auto new_env = scope::new_enclosed(env);
        return eval(expected_ast->get(), new_env);
    }

}
