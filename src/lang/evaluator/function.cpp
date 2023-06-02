#include "evaluator/function.hpp"

namespace arti::lang {

    function::function(const function &)
        : body(nullptr) 
        , env(nullptr)
        , parameters() { }

    function &function::operator=(const function &) {
        return *this;
    }

}
