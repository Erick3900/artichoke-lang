#include "evaluator/native_fns.hpp"

#include <iostream>

#include "evaluator/object.hpp"

namespace arti::lang::native_fns {


    expected_t<types::object_t> read_line(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() != 0) {
            return error_t{ "Number of arguments mismatch" };
        }
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

}
