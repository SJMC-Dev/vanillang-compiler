#include "PrimitiveType.hpp"

namespace vnlc {
    PrimitiveType::PrimitiveType(PrimitiveTypeKind primitiveKind) : SemanticType(), primitiveKind(primitiveKind) {}

    PrimitiveTypeKind PrimitiveType::getPrimitiveKind() const noexcept {
        return primitiveKind;
    }

    std::string_view PrimitiveType::getFullTypeName() const noexcept {
        switch (primitiveKind) {
            case PrimitiveTypeKind::BYTE:
                return "byte";
            case PrimitiveTypeKind::SHORT:
                return "short";
            case PrimitiveTypeKind::INT:
                return "int";
            case PrimitiveTypeKind::LONG:
                return "long";
            case PrimitiveTypeKind::FLOAT:
                return "float";
            case PrimitiveTypeKind::DOUBLE:
                return "double";
            case PrimitiveTypeKind::BOOLEAN:
                return "bool";
            case PrimitiveTypeKind::STRING:
                return "string";
        }

        return "unknown"; // should never reach here
    }

    const PrimitiveType* PrimitiveType::BYTE_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::SHORT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::INT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::LONG_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::FLOAT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::DOUBLE_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::BOOLEAN_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::STRING_TYPE = nullptr;

    const PrimitiveType* PrimitiveType::byteType() {
        if (BYTE_TYPE == nullptr) {
            BYTE_TYPE = new PrimitiveType(PrimitiveTypeKind::BYTE);
        }
        return BYTE_TYPE;
    }

    const PrimitiveType* PrimitiveType::shortType() {
        if (SHORT_TYPE == nullptr) {
            SHORT_TYPE = new PrimitiveType(PrimitiveTypeKind::SHORT);
        }
        return SHORT_TYPE;
    }

    const PrimitiveType* PrimitiveType::intType() {
        if (INT_TYPE == nullptr) {
            INT_TYPE = new PrimitiveType(PrimitiveTypeKind::INT);
        }
        return INT_TYPE;
    }

    const PrimitiveType* PrimitiveType::longType() {
        if (LONG_TYPE == nullptr) {
            LONG_TYPE = new PrimitiveType(PrimitiveTypeKind::LONG);
        }
        return LONG_TYPE;
    }

    const PrimitiveType* PrimitiveType::floatType() {
        if (FLOAT_TYPE == nullptr) {
            FLOAT_TYPE = new PrimitiveType(PrimitiveTypeKind::FLOAT);
        }
        return FLOAT_TYPE;
    }

    const PrimitiveType* PrimitiveType::doubleType() {
        if (DOUBLE_TYPE == nullptr) {
            DOUBLE_TYPE = new PrimitiveType(PrimitiveTypeKind::DOUBLE);
        }
        return DOUBLE_TYPE;
    }

    const PrimitiveType* PrimitiveType::booleanType() {
        if (BOOLEAN_TYPE == nullptr) {
            BOOLEAN_TYPE = new PrimitiveType(PrimitiveTypeKind::BOOLEAN);
        }
        return BOOLEAN_TYPE;
    }

    const PrimitiveType* PrimitiveType::stringType() {
        if (STRING_TYPE == nullptr) {
            STRING_TYPE = new PrimitiveType(PrimitiveTypeKind::STRING);
        }
        return STRING_TYPE;
    }
} // namespace vnlc
