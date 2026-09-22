#ifndef VNLC_PRIMITIVE_TYPE_HPP
#define VNLC_PRIMITIVE_TYPE_HPP

#include "type/PrimitiveTypeKind.hpp"
#include "type/SemanticType.hpp"

namespace vnlc {
    class PrimitiveType : public SemanticType {
    private:
        PrimitiveTypeKind primitiveKind;

        static const PrimitiveType* BYTE_TYPE;
        static const PrimitiveType* SHORT_TYPE;
        static const PrimitiveType* INT_TYPE;
        static const PrimitiveType* LONG_TYPE;
        static const PrimitiveType* FLOAT_TYPE;
        static const PrimitiveType* DOUBLE_TYPE;
        static const PrimitiveType* BOOLEAN_TYPE;
        static const PrimitiveType* STRING_TYPE;
        static const PrimitiveType* OPTIONAL_BYTE_TYPE;
        static const PrimitiveType* OPTIONAL_SHORT_TYPE;
        static const PrimitiveType* OPTIONAL_INT_TYPE;
        static const PrimitiveType* OPTIONAL_LONG_TYPE;
        static const PrimitiveType* OPTIONAL_FLOAT_TYPE;
        static const PrimitiveType* OPTIONAL_DOUBLE_TYPE;
        static const PrimitiveType* OPTIONAL_BOOLEAN_TYPE;
        static const PrimitiveType* OPTIONAL_STRING_TYPE;

        PrimitiveType(PrimitiveTypeKind primitiveKind, bool optional);

    public:
        PrimitiveType(const PrimitiveType&) = delete;
        PrimitiveType(PrimitiveType&&) noexcept = delete;

        [[nodiscard]] PrimitiveTypeKind getPrimitiveKind() const noexcept;
        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;

        [[nodiscard]] static const PrimitiveType* byteType();
        [[nodiscard]] static const PrimitiveType* shortType();
        [[nodiscard]] static const PrimitiveType* intType();
        [[nodiscard]] static const PrimitiveType* longType();
        [[nodiscard]] static const PrimitiveType* floatType();
        [[nodiscard]] static const PrimitiveType* doubleType();
        [[nodiscard]] static const PrimitiveType* booleanType();
        [[nodiscard]] static const PrimitiveType* stringType();
        [[nodiscard]] static const PrimitiveType* optionalByteType();
        [[nodiscard]] static const PrimitiveType* optionalShortType();
        [[nodiscard]] static const PrimitiveType* optionalIntType();
        [[nodiscard]] static const PrimitiveType* optionalLongType();
        [[nodiscard]] static const PrimitiveType* optionalFloatType();
        [[nodiscard]] static const PrimitiveType* optionalDoubleType();
        [[nodiscard]] static const PrimitiveType* optionalBooleanType();
        [[nodiscard]] static const PrimitiveType* optionalStringType();
    };
} // namespace vnlc

#endif // VNLC_PRIMITIVE_TYPE_HPP
