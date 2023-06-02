#include "evaluator/impls/tree_walker.hpp"

#include <concepts>

namespace arti::lang {

    tree_walker::tree_walker() {
        global = std::make_shared<scope>();
    }

    expected_t<types::object_t> tree_walker::eval(ast_node *node) {
        return eval(node, global);
    }

    expected_t<types::object_t> tree_walker::eval(ast_node *node_ptr, const std::shared_ptr<scope> &env) {
        return std::visit(
            [&] (auto &&node) -> expected_t<types::object_t> {
                using node_t = std::decay_t<decltype(node)>;

                if constexpr (std::is_same_v<node_t, types::null_t>) {
                    return types::object_t{ node };
                }
                else if constexpr (std::is_same_v<node_t, types::bool_t>) {
                    return types::object_t{ node };
                }
                else if constexpr (std::is_same_v<node_t, types::char_t>) {
                    return types::object_t{ node };
                }
                else if constexpr (std::is_same_v<node_t, types::int_t>) {
                    return types::object_t{ node };
                }
                else if constexpr (std::is_same_v<node_t, types::float_t>) {
                    return types::object_t{ node };
                }
                else if constexpr (std::is_same_v<node_t, types::string_t>) {
                    return types::object_t{ node };
                }
                else if constexpr (std::is_same_v<node_t, ast::hash>) {
                    return eval_hash(node, env);
                }
                else if constexpr (std::is_same_v<node_t, ast::array>) {
                    return eval_array(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::function>) {
                    return eval_fn(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::unary>) {
                    return eval_unary(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::binary>) {
                    return eval_binary(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::assign>) {
                    return eval_assign(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::declaration>) {
                    return eval_declaration(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::call>) {
                    return eval_call(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::index>) {
                    return eval_index(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::if_else>) {
                    return eval_if_else(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::for_loop>) {
                    return eval_for_loop(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::while_loop>) {
                    return eval_while_loop(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::return_value>) {
                    return eval_return(node, env);
				}
                else if constexpr (std::is_same_v<node_t, ast::identifier>) {
                    return resolve(node, env);
				}
                else if constexpr (std::is_same_v<node_t, std::list<ast::ptr>>) {
                    return eval_block(node, env);
                }

                return types::null_t{};
            },
            node_ptr->data
        );
    }

}
