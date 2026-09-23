#include "TypeExpressionType.hpp"

namespace vnlc {
    TypeExpressionType::TypeExpressionType(const Type* expressedType) : expressedType(expressedType) {}

    std::string_view TypeExpressionType::getFullTypeName() const noexcept {
        return expressedType->getFullTypeName();
    }

    const Type* TypeExpressionType::getExpressedType() const noexcept {
        return expressedType;
    }
} // namespace vnlc
