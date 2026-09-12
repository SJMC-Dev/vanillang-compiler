#include "VnlcTypeExpressionType.hpp"

VnlcTypeExpressionType::VnlcTypeExpressionType(const VnlcSemanticType* expressedType)
    : VnlcSemanticType(),
      expressedType(expressedType) {}

std::string_view VnlcTypeExpressionType::getFullTypeName() const noexcept {
    return expressedType->getFullTypeName();
}

const VnlcSemanticType* VnlcTypeExpressionType::getExpressedType() const noexcept {
    return expressedType;
}
