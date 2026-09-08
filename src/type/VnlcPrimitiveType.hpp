#ifndef VNLC_PRIMITIVE_TYPE_HPP
#define VNLC_PRIMITIVE_TYPE_HPP

#include "type/VnlcPrimitiveTypeKind.hpp"
#include "type/VnlcSemanticType.hpp"

class VnlcPrimitiveType : public VnlcSemanticType {
private:
    VnlcPrimitiveTypeKind primitiveKind;

    static const VnlcPrimitiveType* BYTE_TYPE;
    static const VnlcPrimitiveType* SHORT_TYPE;
    static const VnlcPrimitiveType* INT_TYPE;
    static const VnlcPrimitiveType* LONG_TYPE;
    static const VnlcPrimitiveType* FLOAT_TYPE;
    static const VnlcPrimitiveType* DOUBLE_TYPE;
    static const VnlcPrimitiveType* BOOLEAN_TYPE;
    static const VnlcPrimitiveType* STRING_TYPE;

    VnlcPrimitiveType(VnlcPrimitiveTypeKind primitiveKind);

public:
    [[nodiscard]] VnlcPrimitiveTypeKind getPrimitiveKind() const noexcept;
    [[nodiscard]] std::string_view getFullTypeName() const noexcept override;

    [[nodiscard]] static const VnlcPrimitiveType* byteType();
    [[nodiscard]] static const VnlcPrimitiveType* shortType();
    [[nodiscard]] static const VnlcPrimitiveType* intType();
    [[nodiscard]] static const VnlcPrimitiveType* longType();
    [[nodiscard]] static const VnlcPrimitiveType* floatType();
    [[nodiscard]] static const VnlcPrimitiveType* doubleType();
    [[nodiscard]] static const VnlcPrimitiveType* booleanType();
    [[nodiscard]] static const VnlcPrimitiveType* stringType();
};

#endif // VNLC_PRIMITIVE_TYPE_HPP