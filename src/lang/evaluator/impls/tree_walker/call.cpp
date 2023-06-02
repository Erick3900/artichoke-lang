#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_call(const ast::call &call, const std::shared_ptr<scope> &env) {
        auto expected_fn = eval(call.function.get(), env);

        if (not expected_fn) {
            return expected_fn;
        }

        auto fn_obj = std::move(expected_fn).value();

        if (fn_obj.is<types::native_fn_t>()) {
            std::list<object> params;

            for (const auto &args : call.arguments) {
                auto expected_arg = eval(args.get(), env);

                if (not expected_arg) {
                    return expected_arg;
                }

                if (not expected_arg->is_reference()) {
                    expected_arg->is_const = false;
                }
                else if (expected_arg->is_reference() and not expected_arg->unwrap().is_object()) {
                    expected_arg = expected_arg->unwrap();
                }

                params.emplace_back(std::move(expected_arg).value());
            }

            auto expected_call = fn_obj.as<types::native_fn_t>()(
                std::move(params), 
                [&] (ast_node *p_n, const std::shared_ptr<scope> &p_s) { 
                    return eval(p_n, p_s); 
                }, 
                env
            );

            if (not expected_call) {
                return expected_call;
            }

            return std::move(expected_call).value();
        }
        else {
            if (not (fn_obj.is_reference() and fn_obj.unwrap().is<types::fn_t>())) {
                return error_t{ fmt::format("Expected a function type, got: {}", fn_obj.type()) };
            }

            auto &fn = fn_obj.unwrap().as<types::fn_t>();

            auto new_env = scope::new_enclosed(fn.env);

            if (call.arguments.size() > fn.parameters.size()) {
                return error_t{ "Number of arguments mismatch" };
            }

            auto params_it = fn.parameters.begin();

            for (const auto &args : call.arguments) {
                auto expected_arg = eval(args.get(), env);

                if (not expected_arg) {
                    return expected_arg;
                }

                if (not expected_arg->is_reference()) {
                    expected_arg->is_const = false;
                }
                else if (expected_arg->is_reference() and not expected_arg->unwrap().is_object()) {
                    expected_arg = expected_arg->unwrap();
                }

                new_env->set(*params_it++, std::move(expected_arg).value());
            }

            while (params_it != fn.parameters.end()) {
                new_env->set(*params_it++, types::null_t{});
            }

            auto expected_ret = eval(fn.body.get(), new_env);

            if (not expected_ret) {
                return expected_ret;
            }

            expected_ret->is_return = false;

            return std::move(expected_ret).value();
        }
	}

}
