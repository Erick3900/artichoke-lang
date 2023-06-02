#pragma once

#include "types.hpp"

namespace arti::lang::native_fns {

    expected_t<types::object_t> len(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> fmt(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> copy(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> move(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> eval(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> push(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> import(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> typeof(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> printf(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> resize(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> reserve(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> read_line(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);
    expected_t<types::object_t> to_string(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env);

}
