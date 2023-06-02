#include "evaluator/native_fns.hpp"

#include "evaluator/object.hpp"

namespace arti::lang::native_fns {

    expected_t<types::object_t> reserve(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() < 2 || params.size() > 3) {
            return error_t{ "Number of arguments mismatch" };
        }

        auto it = params.begin();

        auto &obj = *it;

        if (not (obj.is_reference() and obj.unwrap().is<types::array_t>())) {
            return error_t{ fmt::format("Resize is just for arrays, got: '{}'", obj.type()) };
        }

        auto &unw_obj = obj.unwrap().as<types::array_t>();
        auto &new_size = *(++it);

        if (not new_size.is<types::int_t>()) {
            return error_t{ "Reserve expected an integer as second paramater" };
        }

        unw_obj.reserve(new_size.as<types::int_t>());

        return types::null_t{};
    }

}
