#include "PrimitiveType.hpp"

namespace vnlc {
    PrimitiveType::PrimitiveType(PrimitiveTypeKind primitiveKind, bool optional) : SemanticType(optional), primitiveKind(primitiveKind) {}

    PrimitiveTypeKind PrimitiveType::getPrimitiveKind() const noexcept {
        return primitiveKind;
    }

    std::string_view PrimitiveType::getFullTypeName() const noexcept {
        switch (primitiveKind) {
            case PrimitiveTypeKind::BYTE:
                return isOptional() ? "byte?" : "byte";
            case PrimitiveTypeKind::SHORT:
                return isOptional() ? "short?" : "short";
            case PrimitiveTypeKind::INT:
                return isOptional() ? "int?" : "int";
            case PrimitiveTypeKind::LONG:
                return isOptional() ? "long?" : "long";
            case PrimitiveTypeKind::FLOAT:
                return isOptional() ? "float?" : "float";
            case PrimitiveTypeKind::DOUBLE:
                return isOptional() ? "double?" : "double";
            case PrimitiveTypeKind::BOOLEAN:
                return isOptional() ? "bool?" : "bool";
            case PrimitiveTypeKind::STRING:
                return isOptional() ? "string?" : "string";
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
    const PrimitiveType* PrimitiveType::OPTIONAL_BYTE_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::OPTIONAL_SHORT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::OPTIONAL_INT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::OPTIONAL_LONG_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::OPTIONAL_FLOAT_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::OPTIONAL_DOUBLE_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::OPTIONAL_BOOLEAN_TYPE = nullptr;
    const PrimitiveType* PrimitiveType::OPTIONAL_STRING_TYPE = nullptr;

    const PrimitiveType* PrimitiveType::byteType() {
        if (BYTE_TYPE == nullptr) {
            BYTE_TYPE = new PrimitiveType(PrimitiveTypeKind::BYTE, false);
        }
        return BYTE_TYPE;
    }

    const PrimitiveType* PrimitiveType::shortType() {
        if (SHORT_TYPE == nullptr) {
            SHORT_TYPE = new PrimitiveType(PrimitiveTypeKind::SHORT, false);
        }
        return SHORT_TYPE;
    }

    const PrimitiveType* PrimitiveType::intType() {
        if (INT_TYPE == nullptr) {
            INT_TYPE = new PrimitiveType(PrimitiveTypeKind::INT, false);
        }
        return INT_TYPE;
    }

    const PrimitiveType* PrimitiveType::longType() {
        if (LONG_TYPE == nullptr) {
            LONG_TYPE = new PrimitiveType(PrimitiveTypeKind::LONG, false);
        }
        return LONG_TYPE;
    }

    const PrimitiveType* PrimitiveType::floatType() {
        if (FLOAT_TYPE == nullptr) {
            FLOAT_TYPE = new PrimitiveType(PrimitiveTypeKind::FLOAT, false);
        }
        return FLOAT_TYPE;
    }

    const PrimitiveType* PrimitiveType::doubleType() {
        if (DOUBLE_TYPE == nullptr) {
            DOUBLE_TYPE = new PrimitiveType(PrimitiveTypeKind::DOUBLE, false);
        }
        return DOUBLE_TYPE;
    }

    const PrimitiveType* PrimitiveType::booleanType() {
        if (BOOLEAN_TYPE == nullptr) {
            BOOLEAN_TYPE = new PrimitiveType(PrimitiveTypeKind::BOOLEAN, false);
        }
        return BOOLEAN_TYPE;
    }

    const PrimitiveType* PrimitiveType::stringType() {
        if (STRING_TYPE == nullptr) {
            STRING_TYPE = new PrimitiveType(PrimitiveTypeKind::STRING, false);
        }
        return STRING_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalByteType() {
        if (OPTIONAL_BYTE_TYPE == nullptr) {
            OPTIONAL_BYTE_TYPE = new PrimitiveType(PrimitiveTypeKind::BYTE, true);
        }
        return OPTIONAL_BYTE_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalShortType() {
        if (OPTIONAL_SHORT_TYPE == nullptr) {
            OPTIONAL_SHORT_TYPE = new PrimitiveType(PrimitiveTypeKind::SHORT, true);
        }
        return OPTIONAL_SHORT_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalIntType() {
        if (OPTIONAL_INT_TYPE == nullptr) {
            OPTIONAL_INT_TYPE = new PrimitiveType(PrimitiveTypeKind::INT, true);
        }
        return OPTIONAL_INT_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalLongType() {
        if (OPTIONAL_LONG_TYPE == nullptr) {
            OPTIONAL_LONG_TYPE = new PrimitiveType(PrimitiveTypeKind::LONG, true);
        }
        return OPTIONAL_LONG_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalFloatType() {
        if (OPTIONAL_FLOAT_TYPE == nullptr) {
            OPTIONAL_FLOAT_TYPE = new PrimitiveType(PrimitiveTypeKind::FLOAT, true);
        }
        return OPTIONAL_FLOAT_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalDoubleType() {
        if (OPTIONAL_DOUBLE_TYPE == nullptr) {
            OPTIONAL_DOUBLE_TYPE = new PrimitiveType(PrimitiveTypeKind::DOUBLE, true);
        }
        return OPTIONAL_DOUBLE_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalBooleanType() {
        if (OPTIONAL_BOOLEAN_TYPE == nullptr) {
            OPTIONAL_BOOLEAN_TYPE = new PrimitiveType(PrimitiveTypeKind::BOOLEAN, true);
        }
        return OPTIONAL_BOOLEAN_TYPE;
    }

    const PrimitiveType* PrimitiveType::optionalStringType() {
        if (OPTIONAL_STRING_TYPE == nullptr) {
            OPTIONAL_STRING_TYPE = new PrimitiveType(PrimitiveTypeKind::STRING, true);
        }
        return OPTIONAL_STRING_TYPE;
    }
} // namespace vnlc
