#include "evaluator/native_fns.hpp"

#include "evaluator/object.hpp"

namespace arti::lang::native_fns {


    expected_t<types::object_t> copy(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() != 1) {
            return error_t{ "Number of arguments mismatch" };
        }

        auto &obj = params.front();

        if (not obj.is_reference()) {
            return error_t{ "Copy expected a reference value" };
        }

        auto &unw_obj = obj.unwrap();

        if (unw_obj.is<types::fn_t>()) {
            return error_t{ "Functions are not copyables" };
        }

        return unw_obj;
    }

}
