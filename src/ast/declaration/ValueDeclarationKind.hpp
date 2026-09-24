#ifndef VNLC_VALUE_DECLARATION_KIND_HPP
#define VNLC_VALUE_DECLARATION_KIND_HPP

namespace vnlc {
    namespace ValueDeclarationKind {
        enum class Kind {
            LET,
            LOOP_VARIABLE,
            PARAMETER,
            INSTANCE_PROPERTY,
            STATIC_PROPERTY,
            ENUM_ASSOCIATED_VALUE,
        };

        enum class Context {
            TOP_LEVEL,
            FUNCTION,
            CLASS,
            INTERFACE,
            ENUM_MEMBER,
            BLOCK,
        };

        enum class AccessModifier {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };
    }; // namespace ValueDeclarationKind
} // namespace vnlc

#endif // VNLC_VALUE_DECLARATION_KIND_HPP
