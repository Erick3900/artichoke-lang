#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    expected_t<types::object_t> tree_walker::eval_binary(const ast::binary &expr, const std::shared_ptr<scope> &env) {
        auto expected_left = eval(expr.left.get(), env);

        if (not expected_left) {
            return expected_left;
        }

        auto left = [&] () -> object& {
            if (expected_left->is_reference()) {
                return expected_left->unwrap();
            }
            return expected_left.value();
        }();

        if (expr.op == ast::operators::boolean_or) {
            if (left.is_truthy()) {
                return true;
            }
        }
        else if (expr.op == ast::operators::boolean_and) {
            if (not left.is_truthy()) {
                return false;
            }
        }

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

        if (expr.op == ast::operators::boolean_or ||
            expr.op == ast::operators::boolean_and) {
            return right.is_truthy();
        }

        switch (expr.op) {
            case ast::operators::bit_or:
                if (left.is<types::int_t>() and right.is<types::int_t>())
                    return types::int_t{ left.as<types::int_t>() | right.as<types::int_t>() };
                return error_t{ fmt::format("Invalid operands to binary expression, '{} | {}'", left.type(), right.type()) };
            	break;
            case ast::operators::bit_xor:
                if (left.is<types::int_t>() and right.is<types::int_t>())
                    return types::int_t{ left.as<types::int_t>() ^ right.as<types::int_t>() };
                return error_t{ fmt::format("Invalid operands to binary expression, '{} ^ {}'", left.type(), right.type()) };
            	break;
            case ast::operators::bit_and:
                if (left.is<types::int_t>() and right.is<types::int_t>())
                    return types::int_t{ left.as<types::int_t>() & right.as<types::int_t>() };
                return error_t{ fmt::format("Invalid operands to binary expression, '{} & {}'", left.type(), right.type()) };
            	break;

            case ast::operators::bit_left_shift:
                if (left.is<types::int_t>() and right.is<types::int_t>())
                    return types::int_t{ left.as<types::int_t>() << right.as<types::int_t>() };
                return error_t{ fmt::format("Invalid operands to binary expression, '{} << {}'", left.type(), right.type()) };
            	break;
            case ast::operators::bit_right_shift:
                if (left.is<types::int_t>() and right.is<types::int_t>())
                    return types::int_t{ left.as<types::int_t>() >> right.as<types::int_t>() };
                return error_t{ fmt::format("Invalid operands to binary expression, '{} >> {}'", left.type(), right.type()) };
            	break;

            case ast::operators::equal:
                if (left.is<types::null_t>() and right.is<types::null_t>()) {
                    return true;
                }
                if (left.is<types::bool_t>() and right.is<types::bool_t>()) {
                    return left.as<types::bool_t>() == right.as<types::bool_t>();
                }
                else if (left.is<types::int_t>() and right.is<types::int_t>()) {
                    return left.as<types::int_t>() == right.as<types::int_t>();
                }
                else if (left.is<types::float_t>()) {
                    return left.as<types::float_t>() == right.as<types::float_t>();
                }
                else if (left.is<types::string_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::string_t>() == right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::string_t>() == right.as<types::string_t>();
                }
                else if (left.is<types::char_t>() and right.is<types::char_t>()) {
                    return left.as<types::char_t>() == right.as<types::char_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} == {}'", left.type(), right.type()) };
            	break;
            case ast::operators::not_equal:
                if (left.is<types::null_t>()) {
                    return not right.is<types::null_t>();
                }
                else if (right.is<types::null_t>()) {
                    return not left.is<types::null_t>();
                }
                else if (left.is<types::bool_t>() and right.is<types::bool_t>()) {
                    return left.as<types::bool_t>() != right.as<types::bool_t>();
                }
                else if (left.is<types::int_t>() and right.is<types::int_t>()) {
                    return left.as<types::int_t>() != right.as<types::int_t>();
                }
                else if (left.is<types::float_t>()) {
                    return left.as<types::float_t>() != right.as<types::float_t>();
                }
                else if (left.is<types::string_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::string_t>() != right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::string_t>() != right.as<types::string_t>();
                }
                else if (left.is<types::char_t>() and right.is<types::char_t>()) {
                    return left.as<types::char_t>() != right.as<types::char_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} != {}'", left.type(), right.type()) };
            	break;

            case ast::operators::less_than:
                if (left.is<types::bool_t>() and right.is<types::bool_t>()) {
                    return left.as<types::bool_t>() < right.as<types::bool_t>();
                }
                else if (left.is<types::int_t>() and right.is<types::int_t>()) {
                    return left.as<types::int_t>() < right.as<types::int_t>();
                }
                else if (left.is<types::float_t>()) {
                    return left.as<types::float_t>() < right.as<types::float_t>();
                }
                else if (left.is<types::string_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::string_t>() < right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::string_t>() < right.as<types::string_t>();
                }
                else if (left.is<types::char_t>() and right.is<types::char_t>()) {
                    return left.as<types::char_t>() < right.as<types::char_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} < {}'", left.type(), right.type()) };
            	break;
            case ast::operators::less_than_equal:
                if (left.is<types::bool_t>() and right.is<types::bool_t>()) {
                    return left.as<types::bool_t>() <= right.as<types::bool_t>();
                }
                else if (left.is<types::int_t>() and right.is<types::int_t>()) {
                    return left.as<types::int_t>() <= right.as<types::int_t>();
                }
                else if (left.is<types::float_t>()) {
                    return left.as<types::float_t>() <= right.as<types::float_t>();
                }
                else if (left.is<types::string_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::string_t>() <= right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::string_t>() <= right.as<types::string_t>();
                }
                else if (left.is<types::char_t>() and right.is<types::char_t>()) {
                    return left.as<types::char_t>() <= right.as<types::char_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} <= {}'", left.type(), right.type()) };
            	break;

            case ast::operators::greater_than:
                if (left.is<types::bool_t>() and right.is<types::bool_t>()) {
                    return left.as<types::bool_t>() > right.as<types::bool_t>();
                }
                else if (left.is<types::int_t>() and right.is<types::int_t>()) {
                    return left.as<types::int_t>() > right.as<types::int_t>();
                }
                else if (left.is<types::float_t>()) {
                    return left.as<types::float_t>() > right.as<types::float_t>();
                }
                else if (left.is<types::string_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::string_t>() > right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::string_t>() > right.as<types::string_t>();
                }
                else if (left.is<types::char_t>() and right.is<types::char_t>()) {
                    return left.as<types::char_t>() > right.as<types::char_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} > {}'", left.type(), right.type()) };
                break;
            case ast::operators::greater_than_equal:
                if (left.is<types::bool_t>() and right.is<types::bool_t>()) {
                    return left.as<types::bool_t>() >= right.as<types::bool_t>();
                }
                else if (left.is<types::int_t>() and right.is<types::int_t>()) {
                    return left.as<types::int_t>() >= right.as<types::int_t>();
                }
                else if (left.is<types::float_t>()) {
                    return left.as<types::float_t>() >= right.as<types::float_t>();
                }
                else if (left.is<types::string_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::string_t>() >= right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::string_t>() >= right.as<types::string_t>();
                }
                else if (left.is<types::char_t>() and right.is<types::char_t>()) {
                    return left.as<types::char_t>() >= right.as<types::char_t>();
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} >= {}'", left.type(), right.type()) };
            	break;

            case ast::operators::addition:
                if (left.is<types::bool_t>()) {
                    if (right.is<types::bool_t>())
                        return types::int_t{ left.as<types::bool_t>() + right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::int_t{ left.as<types::bool_t>() + right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::bool_t>() + right.as<types::float_t>() };
                    else if (right.is<types::char_t>())
                        return static_cast<types::char_t>( left.as<types::bool_t>() + right.as<types::char_t>() );
                }
                else if (left.is<types::int_t>()) {
                    if (right.is<types::bool_t>())
                        return types::int_t{ left.as<types::int_t>() + right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::int_t{ left.as<types::int_t>() + right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::int_t>() + right.as<types::float_t>() };
                    else if (right.is<types::char_t>())
                        return static_cast<types::char_t>( left.as<types::int_t>() + right.as<types::char_t>() );
                }
                else if (left.is<types::float_t>()) {
                    if (right.is<types::bool_t>())
                        return types::float_t{ left.as<types::float_t>() + right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::float_t{ left.as<types::float_t>() + right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::float_t>() + right.as<types::float_t>() };
                }
                else if (left.is<types::string_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::string_t>() + right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::string_t>() + right.as<types::string_t>();
                    else if (right.is<types::char_t>())
                        return left.as<types::string_t>() + right.as<types::char_t>();
                }
                else if (left.is<types::char_t>()) {
                    if (right.is_reference() and right.unwrap().is<types::string_t>())
                        return left.as<types::char_t>() + right.unwrap().as<types::string_t>();
                    else if (right.is<types::string_t>())
                        return left.as<types::char_t>() + right.as<types::string_t>();
                    else if (right.is<types::int_t>())
                        return static_cast<types::char_t>( left.as<types::char_t>() + right.as<types::int_t>() );
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} + {}'", left.type(), right.type()) };
            	break;
            case ast::operators::substraction:
                if (left.is<types::bool_t>()) {
                    if (right.is<types::bool_t>())
                        return types::int_t{ left.as<types::bool_t>() - right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::int_t{ left.as<types::bool_t>() - right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::bool_t>() - right.as<types::float_t>() };
                }
                else if (left.is<types::int_t>()) {
                    if (right.is<types::bool_t>())
                        return types::int_t{ left.as<types::int_t>() - right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::int_t{ left.as<types::int_t>() - right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::int_t>() - right.as<types::float_t>() };
                }
                else if (left.is<types::float_t>()) {
                    if (right.is<types::bool_t>())
                        return types::float_t{ left.as<types::float_t>() - right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::float_t{ left.as<types::float_t>() - right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::float_t>() - right.as<types::float_t>() };
                }
                else if (left.is<types::char_t>())
                    if (right.is<types::int_t>())
                        return static_cast<types::char_t>( left.as<types::char_t>() - right.as<types::int_t>() );
                return error_t{ fmt::format("Invalid operands to binary expression, '{} - {}'", left.type(), right.type()) };
            	break;

            case ast::operators::modulo:
                if (left.is<types::int_t>() and right.is<types::int_t>())
                    return types::int_t{ left.as<types::int_t>() % right.as<types::int_t>() };
                return error_t{ fmt::format("Invalid operands to binary expression, '{} % {}'", left.type(), right.type()) };
            	break;
            case ast::operators::division:
                if (left.is<types::int_t>()) {
                    if (right.is<types::int_t>())
                        return types::int_t{ left.as<types::int_t>() / right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::int_t>() / right.as<types::float_t>() };
                }
                else if (left.is<types::float_t>()) {
                    if (right.is<types::int_t>())
                        return types::float_t{ left.as<types::float_t>() / right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::float_t>() / right.as<types::float_t>() };
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} / {}'", left.type(), right.type()) };
            	break;
            case ast::operators::multiplication:
                if (left.is<types::bool_t>()) {
                    if (right.is<types::bool_t>())
                        return types::int_t{ left.as<types::bool_t>() * right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::int_t{ left.as<types::bool_t>() * right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::bool_t>() * right.as<types::float_t>() };
                }
                else if (left.is<types::int_t>()) {
                    if (right.is<types::bool_t>())
                        return types::int_t{ left.as<types::int_t>() * right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::int_t{ left.as<types::int_t>() * right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::int_t>() * right.as<types::float_t>() };
                }
                else if (left.is<types::float_t>()) {
                    if (right.is<types::bool_t>())
                        return types::float_t{ left.as<types::float_t>() * right.as<types::bool_t>() };
                    else if (right.is<types::int_t>())
                        return types::float_t{ left.as<types::float_t>() * right.as<types::int_t>() };
                    else if (right.is<types::float_t>())
                        return types::float_t{ left.as<types::float_t>() * right.as<types::float_t>() };
                }
                return error_t{ fmt::format("Invalid operands to binary expression, '{} * {}'", left.type(), right.type()) };
            	break;
            default: return error_t{ "Unknown operator" };
        }
        return types::null_t{};
	}

}
