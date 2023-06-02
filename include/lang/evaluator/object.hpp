#pragma once

#include <ranges>

#include <fmt/core.h>
#include <fmt/format.h>

#include "error.hpp"
#include "utils/overload_set.hpp"

#include "parser/ast.hpp"

#include "evaluator/function.hpp"

namespace arti::lang {

    struct object {
        bool is_const;
        bool is_return;

        std::variant<
            types::null_t,
            types::bool_t,
            types::char_t,
            types::int_t,
            types::float_t,
            types::string_t,
            types::array_t,
            types::hash_t,
            types::fn_t,
            types::native_fn_t,
            types::reference_t
        > value;

        object();

        object(types::null_t);

        object(types::char_t v);
        object(types::int_t v);
        object(types::bool_t v);
        object(types::float_t v);
        object(types::reference_t v);
        object(types::hash_t v);
        object(types::array_t v);
        object(types::string_t v);
        object(types::native_fn_t v);

        ~object() = default;

        object(object &&) = default;
        object(const object &) = default;

        object &operator=(object &&) = default;
        object &operator=(const object &) = default;

        bool is_object() const;
        bool is_reference() const;
        bool is_truthy() const;

        object &unwrap();
        const object &unwrap() const;

        std::string type() const;
        std::string to_string() const;

        template <typename T>
        bool is() const {
            return std::holds_alternative<T>(value);
        }

        template <typename T>
        T &as() {
            return std::get<T>(value);
        }

        template <typename T>
        const T &as() const {
            return std::get<T>(value);
        }
    };
}

template<> 
struct fmt::formatter<arti::lang::object> {
    std::string format_str;

    auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
        auto it = ctx.begin();
        auto end = ctx.end();

        while (it != end && *it != '}') {
            ++it;
        }

        format_str = fmt::format("{{:{}}}", std::string_view{ ctx.begin(), it });

        return it;
    }

    auto format(const arti::lang::object& obj, format_context& ctx) const ->  decltype(ctx.out()) {
        using namespace arti::lang;
        return std::visit(arti::overload_set{
            [&](types::null_t){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), "null");
            },
            [&](types::bool_t v){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), v);
            },
            [&](types::char_t v){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), v);
            },
            [&](types::int_t v){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), v);
            },
            [&](types::float_t v){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), v);
            },
            [&](const types::string_t &v){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), v);
            },
            [&](const types::array_t &v){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), fmt::format(
                    "[{}]", 
                    fmt::join(v, ", ")
                ));
            },
            [&](const types::hash_t &v){
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), fmt::format(
                    "{{{}}}",
                    fmt::join(
                        std::views::transform(v, [] (const auto &e) {
                              const auto &[key, val] = e;
                              return fmt::format("{}: {}", key, val);
                          }
                        ),
                        ", "
                    )
                ));
            },
            [&](const types::fn_t &) {
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), "[Object function]");
            },
            [&](const types::native_fn_t &) {
                return fmt::format_to(ctx.out(), fmt::runtime(format_str), "[Native function]");
            },
            [&](types::reference_t obj) -> decltype(ctx.out()) {
                return fmt::format_to(ctx.out(), "{}", fmt::format(fmt::runtime(format_str), *obj));
            }
        }, obj.value);
    }

};

