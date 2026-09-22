#include "VoidType.hpp"

namespace vnlc {
    VoidType::VoidType() : SemanticType(false) {}

    std::string_view VoidType::getFullTypeName() const noexcept {
        return "void";
    }
} // namespace vnlc
