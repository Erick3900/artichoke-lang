#include "evaluator/native_fns.hpp"

#include <fmt/args.h>

#include "evaluator/object.hpp"

namespace arti::lang::native_fns {

    expected_t<types::object_t> fmt(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() < 1) {
            return error_t{ "Number of arguments mismatch" };
        }

        auto it = params.begin();

        auto format_str = std::move(*it);

        if (not format_str.is<types::string_t>()) {
            return error_t{ "Expected first parameter to be an format string" };
        }

        params.pop_front();

        fmt::dynamic_format_arg_store<fmt::format_context> store;

        for (auto &obj : params) {
            store.push_back(obj);
        }

        try {
            return fmt::vformat(format_str.as<types::string_t>(), store);
        }
        catch(std::exception &e) {
            return error_t{ e.what() };
        }

        return types::null_t{};
    }

}
