#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_unary(const ast::unary &expr, const std::shared_ptr<scope> &env) {
        auto expected_right = eval(expr.right.get(), env);

        if (not expected_right) {
            return expected_right;
        }

        auto right = [&] () -> object& {
            if (expected_right->is_reference()) {
                return expected_right->unwrap();
            }
            return expected_right.value();
        }();

        switch  (expr.op) {
            case ast::operators::substraction:
                if (right.is<types::int_t>()) {
                    return -right.as<types::int_t>();
                }
                else if (right.is<types::float_t>()) {
                    return -right.as<types::float_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '-{}'", right.type()) };
                break;
            case ast::operators::boolean_not:
                if (right.is<types::int_t>()) {
                    return not right.as<types::int_t>();
                }
                else if (right.is<types::float_t>()) {
                    return not right.as<types::float_t>();
                }
                else if (right.is<types::null_t>()) {
                    return true;
                }
                else if (right.is<types::bool_t>()) {
                    return not right.as<types::bool_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, 'not {}'", right.type()) };
                break;
            case ast::operators::bit_not:
                if (right.is<types::int_t>()) {
                    return ~right.as<types::int_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '~{}'", right.type()) };
            	break;
            default: return error_t{ "Unknown operator" };
        }
        return types::null_t{};
	}

}
