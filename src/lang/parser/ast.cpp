#include "parser/ast.hpp"

#include <ranges>
#include <sstream>

#include <fmt/format.h>

#include "utils/overload_set.hpp"

namespace arti::lang {

    namespace ast {

        inline constexpr std::string_view operator_to_string(operators op) {
            switch(op) {
                case operators::bit_or: return "bit_or";
                case operators::bit_xor: return "bit_xor";
                case operators::bit_and: return "bit_and";
                case operators::bit_not: return "bit_not";

                case operators::bit_left_shift: return "bit_left_shift";
                case operators::bit_right_shift: return "bit_right_shift";

                case operators::boolean_or: return "boolean_or";
                case operators::boolean_not: return "boolean_not";
                case operators::boolean_and: return "boolean_and";

                case operators::equal: return "equal";
                case operators::not_equal: return "not_equal";

                case operators::less_than: return "less_than";
                case operators::less_than_equal: return "less_than_equal";

                case operators::greater_than: return "greater_than";
                case operators::greater_than_equal: return "greater_than_equal";

                case operators::addition: return "addition";
                case operators::substraction: return "substraction";

                case operators::modulo: return "modulo";
                case operators::division: return "division";
                case operators::multiplication: return "multiplication";
            }
            return "<unknown>";
        }

        std::string hash_pair::dump(std::string indent) const {
            return fmt::format(
                "{}hash_pair\n{}key\n{}\n{}value\n{}",
                indent,
                indent + "  ",
                key->dump(indent + "    "),
                indent + "  ",
                value->dump(indent + "    ")
            );
		}

        std::string hash::dump(std::string indent) const {
            if (elements.size() == 0) {
                return fmt::format("{}hash_table\n{}[]", indent, indent + "  ");
            }

            return fmt::format(
                "{}hash_table\n{}",
                indent,
                fmt::join(
                    std::views::transform(
                        elements,
                        [&] (const auto &el) {
                            return el.dump(indent + "  ");
                        }
                    ),
                    "\n"
                )
            );
        }

        std::string array::dump(std::string indent) const {
            if (elements.size() == 0) {
                return fmt::format("{}array\n{}[]", indent, indent + "  ");
            }

            return fmt::format(
                "{}array\n{}",
                indent,
                fmt::join(
                    std::views::transform(
                        elements,
                        [&] (const auto &el) {
                            return el->dump(indent + "  ");
                        }
                    ),
                    "\n"
                )
            );
		}

        std::string function::dump(std::string indent) const {
            return fmt::format(
                "{}function_literal\n{}parameters\n{}\n{}body\n{}",
                indent,
                indent + "  ",
                fmt::join(
                    std::views::transform(
                        parameters,
                        [&] (const auto &param) {
                            return indent + "    " + param;
                        }
                    ),
                    "\n"
                ),
                indent + "  ",
                body->dump(indent + "    ")
            );
		}

        std::string unary::dump(std::string indent) const {
            return fmt::format(
                "{}unary_expression\n{}operator {}\n{}expression\n{}",
                indent,
                indent + "  ",
                operator_to_string(op),
                indent + "  ",
                right->dump(indent + "     ")
            );
		}

        std::string binary::dump(std::string indent) const {
            return fmt::format(
                "{}binary_expression\n{}operator {}\n{}left_expression\n{}\n{}right_expression\n{}",
                indent,
                indent + "  ",
                operator_to_string(op),
                indent + "  ",
                left->dump(indent + "     "),
                indent + "  ",
                right->dump(indent + "     ")
            );
		}

        std::string assign::dump(std::string indent) const {
		    return fmt::format(
                "{}assign_expression\n{}variable\n{}\n{}value\n{}",
                indent,
                indent + "  ",
                variable->dump(indent + "     "),
                indent + "  ",
                value->dump(indent + "     ")
            );
        }

        std::string declaration::dump(std::string indent) const {
		    return fmt::format(
                "{}declaration\n{}is_const {}\n{}variable {}\n{}value\n{}",
                indent,
                indent + "  ",
                is_const,
                indent + "  ",
                variable,
                indent + "  ",
                value->dump(indent + "     ")
            );
        }

        std::string call::dump(std::string indent) const {
		    return fmt::format(
                "{}call\n{}function\n{}\n{}arguments\n{}",
                indent,
                indent + "  ",
                function->dump(indent + "    "),
                indent + "  ",
                fmt::join(
                    std::views::transform(
                        arguments,
                        [&] (const auto &arg) {
                            return arg->dump(indent + "    ");
                        }
                    ),
                    "\n"
                )
            );
        }

        std::string index::dump(std::string indent) const {
		    return fmt::format(
                "{}index_access\n{}variable\n{}\n{}index\n{}",
                indent,
                indent + "  ",
                variable->dump(indent + "    "),
                indent + "  ",
                index->dump(indent + "    ")
            );
        }

        std::string if_else::dump(std::string indent) const {
		    if (else_branch) {
                return fmt::format(
                    "{}if_expression\n{}condition\n{}\n{}if_branch\n{}\n{}else_branch\n{}",
                    indent,
                    indent + "  ",
                    condition->dump(indent + "    "),
                    indent + "  ",
                    if_branch->dump(indent + "    "),
                    indent + "  ",
                    else_branch->dump(indent + "    ")
                );
            }

            return fmt::format(
                "{}if_expression\n{}condition\n{}\n{}if_branch\n{}",
                indent,
                indent + "  ",
                condition->dump(indent + "    "),
                indent + "  ",
                if_branch->dump(indent + "    ")
            );
        }

        std::string for_loop::dump(std::string indent) const {
            return fmt::format(
                "{}for_loop\n{}before\n{}\n{}condition\n{}\n{}after\n{}\n{}body\n{}",
                indent,
                indent + "  ",
                before
                    ? before->dump(indent + "    ")
                    : "null",
                indent + "  ",
                condition
                    ? condition->dump(indent + "    ")
                    : "null",
                indent + "  ",
                after 
                    ? after->dump(indent + "    ")
                    : "null",
                indent + "  ",
                body->dump(indent + "    ")
            );
        }

        std::string while_loop::dump(std::string indent) const {
            return fmt::format(
                "{}while_loop\n{}condition\n{}\n{}body\n{}",
                indent,
                indent + "  ",
                condition
                    ? condition->dump(indent + "    ")
                    : "null",
                indent + "  ",
                body->dump(indent + "    ")
            );
        }

        std::string return_value::dump(std::string indent) const {
            return fmt::format(
                "{}return\n{}value\n{}",
                indent,
                indent + "  ",
                value
                    ? value->dump(indent + "    ")
                    : "null"
            );
        }

        std::string identifier::dump(std::string indent) const {
            return fmt::format(
                "{}identifier\n{}name '{}'",
                indent,
                indent + "  ",
                name
            );
        }
    }

    std::string ast_node::dump(std::string indent) const {
        return std::visit(
            overload_set{
                [&] (types::null_t) -> std::string {
                    return indent + "null";
                },
                [&] (types::bool_t v) -> std::string {
                    return indent + fmt::to_string(v);
                },
                [&] (types::char_t v) -> std::string {
                    return fmt::format("{}'{}'", indent, [&] () -> std::string{
                        switch(v) {
                            case '\t': return "\\t";
                            case '\b': return "\\b";
                            case '\n': return "\\n";
                            case '\r': return "\\r";
                            case '\f': return "\\f";
                            default:
                                return std::string{ v };
                        }
                    }());
                },
                [&] (types::int_t v) -> std::string {
                    return indent + fmt::to_string(v);
                },
                [&] (types::float_t v) -> std::string {
                    return indent + fmt::to_string(v);
                },
                [&] (const types::string_t &v) -> std::string {
                    return fmt::format("{}\"{}\"", indent, v);
                },
                [&] (const ast::hash &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::array &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::function &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::unary &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::binary &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::assign &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::declaration &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::call &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::index &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::if_else &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::for_loop &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::while_loop &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::return_value &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const ast::identifier &v) -> std::string {
                    return v.dump(indent);
                },
                [&] (const std::list<ptr> &st) -> std::string {
                    return fmt::format(
                        "{}block\n{}",
                        indent,
                        fmt::join(
                            std::views::transform(
                                st,
                                [&] (const auto &s) {
                                    return s->dump(indent + "  ");
                                }
                            ),
                            "\n"
                        )
                    );
                }
            },
            data
        );
    }

}
