#ifndef VNLC_TYPE_EXPRESSION_TYPE_HPP
#define VNLC_TYPE_EXPRESSION_TYPE_HPP

#include "type/Type.hpp"

namespace vnlc {
    class TypeExpressionType : public Type {
    private:
        const Type* expressedType;

    public:
        TypeExpressionType(const Type* expressedType);

        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
        [[nodiscard]] const Type* getExpressedType() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_EXPRESSION_TYPE_HPP
