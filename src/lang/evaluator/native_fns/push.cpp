#include "evaluator/native_fns.hpp"

#include "evaluator/object.hpp"

namespace arti::lang::native_fns {


    expected_t<types::object_t> push(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() != 2) {
            return error_t{ "Number of arguments mismatch" };
        }

        auto it = params.begin();

        auto &obj = *it;

        if (not obj.is_reference() or obj.is_const) {
            return error_t{ "Push expected a mutable reference" };
        }

        if (not (obj.unwrap().is<types::string_t>() or obj.unwrap().is<types::array_t>())) {
            return error_t{ "Push is just for arrays or strings" };
        }

        auto &unw_obj = obj.unwrap();
        auto &value = *(++it);

        if (unw_obj.is<types::array_t>()) {
            unw_obj.as<types::array_t>().emplace_back(std::move(value));
        }
        else if (unw_obj.is<types::string_t>()) {
            if (value.is<types::string_t>()) {
                unw_obj.as<types::string_t>().append(std::move(value.as<types::string_t>()));
            }
            else if (value.is<types::char_t>()){
                unw_obj.as<types::string_t>().push_back(value.as<types::char_t>());
            }
            else return error_t{ "Invalid push parameters" };
        }

        return types::null_t{};
    }

}
