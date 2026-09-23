#include "VoidType.hpp"

namespace vnlc {
    std::string_view VoidType::getFullTypeName() const noexcept {
        return "void";
    }
} // namespace vnlc
