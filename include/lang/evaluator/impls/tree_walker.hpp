#pragma once

#include "evaluator/scope.hpp"
#include "evaluator/object.hpp"

namespace arti::lang {

    struct tree_walker {
        std::shared_ptr<scope> global;
        std::unordered_map<std::string_view, types::native_fn_t> builtins;

        tree_walker();

        expected_t<object> eval(ast_node *node);
        expected_t<object> eval(ast_node *node, const std::shared_ptr<scope> &env);

        expected_t<types::object_t> eval_fn(ast::function &fn, const std::shared_ptr<scope> &env);

        expected_t<types::object_t> resolve(const ast::identifier &ident, const std::shared_ptr<scope> &env);

        expected_t<types::object_t> eval_array(const ast::array &arr, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_hash(const ast::hash &hash, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_assign(const ast::assign &assign, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_declaration(const ast::declaration &decl, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_call(const ast::call &call, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_index(const ast::index &index, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_if_else(const ast::if_else &if_else, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_for_loop(const ast::for_loop &for_loop, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_while_loop(const ast::while_loop &while_loop, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_return(const ast::return_value &return_expr, const std::shared_ptr<scope> &env);

        expected_t<types::object_t> eval_block(const std::list<ast::ptr> &statements, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_unary(const ast::unary &expr, const std::shared_ptr<scope> &env);
        expected_t<types::object_t> eval_binary(const ast::binary &expr, const std::shared_ptr<scope> &env);
    };

}
