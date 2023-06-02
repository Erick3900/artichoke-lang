#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_block(const std::list<ast::ptr> &statements, const std::shared_ptr<scope> &env) {
        types::object_t result;

        for (const auto &statement : statements) {
            auto expected_result = eval(statement.get(), env);

            if (not expected_result) {
                return error_t{ std::move(expected_result).error() };
            }

            if (expected_result->is_return) {
                return std::move(expected_result).value();
            }

            result = std::move(expected_result).value();
        }

        if (not statements.back()->had_semicolon) {
            return result;
        }

        return types::null_t{};
	}

}
