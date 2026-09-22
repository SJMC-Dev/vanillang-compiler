#include "SemanticType.hpp"

namespace vnlc {
    SemanticType::SemanticType(bool optional) : optional(optional) {}

    bool SemanticType::isOptional() const noexcept {
        return optional;
    }
} // namespace vnlc
