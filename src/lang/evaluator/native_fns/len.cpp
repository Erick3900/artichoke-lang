#include "evaluator/native_fns.hpp"

#include "evaluator/object.hpp"

namespace arti::lang::native_fns {


    expected_t<types::object_t> len(std::list<types::object_t> params, types::eval_fn_t eval, const std::shared_ptr<scope> &env) {
        if (params.size() != 1) {
            return error_t{ "Number of arguments missmatch" };
        }
        auto &obj = params.front();

        if (obj.is<types::string_t>()) {
            return static_cast<types::int_t>(obj.as<types::string_t>().size());
        }
        if (obj.is<types::array_t>()) {
            return static_cast<types::int_t>(obj.as<types::array_t>().size());
        }
        if (obj.is<types::hash_t>()) {
            return static_cast<types::int_t>(obj.as<types::hash_t>().size());
        }
        if (obj.is_reference()) {
            auto &unw_obj = obj.unwrap();

            if (unw_obj.is<types::string_t>()) {
                return static_cast<types::int_t>(unw_obj.as<types::string_t>().size());
            }
            if (unw_obj.is<types::array_t>()) {
                return static_cast<types::int_t>(unw_obj.as<types::array_t>().size());
            }
            if (unw_obj.is<types::hash_t>()) {
                return static_cast<types::int_t>(unw_obj.as<types::hash_t>().size());
            } 
        }

        return error_t{ fmt::format("Invalid type for built-in len function '{}'", obj.type()) };
    }

}
