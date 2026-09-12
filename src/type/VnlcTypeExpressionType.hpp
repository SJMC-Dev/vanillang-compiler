#ifndef VNLC_TYPE_EXPRESSION_TYPE_HPP
#define VNLC_TYPE_EXPRESSION_TYPE_HPP

#include "type/VnlcSemanticType.hpp"

class VnlcTypeExpressionType : public VnlcSemanticType {
private:
    const VnlcSemanticType* expressedType;

public:
    VnlcTypeExpressionType(const VnlcSemanticType* expressedType);

    [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
    [[nodiscard]] const VnlcSemanticType* getExpressedType() const noexcept;
};

#endif // VNLC_TYPE_EXPRESSION_TYPE_HPP
