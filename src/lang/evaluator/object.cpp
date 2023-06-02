#include "evaluator/object.hpp"

#include <sstream>

namespace arti::lang {

    object::object() : is_const(false), is_return(false), value(types::null_t{}) {}

    object::object(types::null_t) : is_const(false), is_return(false), value(types::null_t{}) {}

    object::object(types::char_t v) : is_const(false), is_return(false), value(v) {}
    object::object(types::int_t v) : is_const(false), is_return(false), value(v) {}
    object::object(types::bool_t v) : is_const(false), is_return(false), value(v) {}
    object::object(types::float_t v) : is_const(false), is_return(false), value(v) {}

    object::object(types::hash_t v) : is_const(false), is_return(false), value(std::move(v)) {}
    object::object(types::array_t v) : is_const(false), is_return(false), value(std::move(v)) {}
    object::object(types::string_t v) : is_const(false), is_return(false), value(std::move(v)) {}
    object::object(types::native_fn_t v) : is_const(false), is_return(false), value(std::move(v)) {}

    object::object(types::reference_t v) : is_const(false), is_return(false), value(v) {}

    bool object::is_object() const {
        return (
            is<types::string_t>() ||
            is<types::array_t>() ||
            is<types::hash_t>() ||
            is<types::fn_t>()
        );
    }

    bool object::is_reference() const {
        return is<types::reference_t>();
    }

    bool object::is_truthy() const {
        return std::visit([] (auto &&obj) -> bool {
            using obj_t = std::decay_t<decltype(obj)>;

            if constexpr (std::is_same_v<obj_t, types::null_t>) {
                return false;
            }
            else if constexpr (std::is_same_v<obj_t, types::bool_t>) {
                return obj;
            }
            else if constexpr (std::is_same_v<obj_t, types::float_t>) {
                return obj != 0.0;
            }
            else if constexpr (std::is_same_v<obj_t, types::int_t>) {
                return obj != 0;
            }
            else if constexpr (std::is_same_v<obj_t, types::string_t>) {
                return not obj.empty();
            }
            else if constexpr (std::is_same_v<obj_t, types::array_t>) {
                return not obj.empty();
            }
            else if constexpr (std::is_same_v<obj_t, types::hash_t>) {
                return not obj.empty();
            }
            else if constexpr (std::is_same_v<obj_t, types::reference_t>) {
                return (*obj).is_truthy();
            }
            else return true;
        }, value);
    }

    object &object::unwrap() {
        return *as<types::reference_t>();
    }

    const object &object::unwrap() const {
        return *as<types::reference_t>();
    }


    std::string object::to_string() const {
        std::stringstream ss;

        std::visit(overload_set{
            [&](types::null_t){
                ss << "null";
            },
            [&](types::bool_t v){
                ss << fmt::format("{}", v);
            },
            [&](types::int_t v){
                ss << v;
            },
            [&](types::float_t v){
                ss << v;
            },
            [&] (types::char_t v) {
                ss << fmt::format("'{}'", [&] () -> std::string{
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
            [&](const types::string_t &v){
                ss << "\"" << v << "\"";
            },
            [&](const types::array_t &v){
                ss << fmt::format(
                    "[{}]", 
                    fmt::join(
                std::views::transform(v, 
                            [] (const auto &e) {
                                return e.to_string();
                            }
                        ), 
                    ", "
                    )
                );
            },
            [&](const types::hash_t &v){
                ss << fmt::format(
                    "{{{}}}",
                    fmt::join(
                    std::views::transform(v,
                            [] (const auto &e) {
                                const auto &[key, val] = e;
                                return fmt::format("{}: {}", key, val.to_string());
                            }
                        ),
                    ", "
                    )
                );
            },
            [&](const types::fn_t &) {
                ss << "[Object function]";
            },
            [&](const types::native_fn_t &) {
                ss << "[Native function]";
            },
            [&](const types::reference_t obj) {
                ss << obj->to_string();
            }
        }, value);

        return ss.str();
    }

    std::string object::type() const {
        return std::visit(overload_set{
            [&](types::null_t) -> std::string { return "null"; },
            [&](types::bool_t) -> std::string { return "bool"; },
            [&](types::char_t) -> std::string { return "character"; },
            [&](types::int_t) -> std::string { return "integer"; },
            [&](types::float_t) -> std::string { return "floating"; },
            [&](const types::hash_t &) -> std::string { return "hash"; },
            [&](const types::array_t &) -> std::string { return "array"; },
            [&](const types::string_t &) -> std::string { return "string"; },
            [&](const types::fn_t &) -> std::string { return "function"; },
            [&](const types::native_fn_t &) -> std::string { return "native_fn"; },
            [&](types::reference_t obj) -> std::string { return fmt::format("ref: {}", obj->type()); },
        }, value);
    }

    std::ostream &operator<<(std::ostream &os, const object &obj) {
        return os << obj.to_string();
    }

}
