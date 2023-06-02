#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_fn(ast::function &ast_fn, const std::shared_ptr<scope> &env) {
        auto obj = object{};
        obj.value = types::fn_t{};

        auto &fn = std::get<types::fn_t>(obj.value);

        for (const auto &param : ast_fn.parameters) {
            fn.parameters.push_back(param);
        }

        fn.body = std::move(ast_fn.body);
        fn.env = env;

        return std::move(obj);
	}

}
