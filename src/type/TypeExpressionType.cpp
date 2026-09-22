#include "TypeExpressionType.hpp"

namespace vnlc {
    TypeExpressionType::TypeExpressionType(const SemanticType* expressedType) : SemanticType(expressedType->isOptional()), expressedType(expressedType) {}

    std::string_view TypeExpressionType::getFullTypeName() const noexcept {
        return expressedType->getFullTypeName();
    }

    const SemanticType* TypeExpressionType::getExpressedType() const noexcept {
        return expressedType;
    }
} // namespace vnlc
