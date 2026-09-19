#ifndef VNLC_TYPE_EXPRESSION_TYPE_HPP
#define VNLC_TYPE_EXPRESSION_TYPE_HPP

#include "type/SemanticType.hpp"

namespace vnlc {
    class TypeExpressionType : public SemanticType {
    private:
        const SemanticType* expressedType;

    public:
        TypeExpressionType(const SemanticType* expressedType);

        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
        [[nodiscard]] const SemanticType* getExpressedType() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_EXPRESSION_TYPE_HPP
