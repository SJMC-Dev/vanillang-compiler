#ifndef VNLC_OPERATOR_DECLARATION_KIND_HPP
#define VNLC_OPERATOR_DECLARATION_KIND_HPP

namespace vnlc {
    namespace OperatorDeclarationKind {
        enum class Kind {
            ADDITION,
            SUBTRACTION,
            MULTIPLICATION,
            DIVISION,
            INTEGER_DIVISION,
            MODULO,
            EXPONENT,
            EQUAL,
            NOT_EQUAL,
            LESS_THAN,
            GREATER_THAN,
            LESS_THAN_OR_EQUAL,
            GREATER_THAN_OR_EQUAL,
            BITWISE_AND,
            BITWISE_OR,
            BITWISE_XOR,
            BITWISE_NOT,
            SHIFT_LEFT,
            SHIFT_RIGHT,
            SHIFT_RIGHT_UNSIGNED,
            CALL,
            SUBSCRIPT,
            RANGE,
        };

        enum class Context {
            CLASS,
            INTERFACE,
        };

        enum class AccessModifier {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };
    }; // namespace OperatorDeclarationKind
} // namespace vnlc

#endif // VNLC_OPERATOR_DECLARATION_KIND_HPP
