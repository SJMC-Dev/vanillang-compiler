#ifndef VNLC_FUNCTION_DECLARATION_KIND_HPP
#define VNLC_FUNCTION_DECLARATION_KIND_HPP

namespace vnlc {
    namespace FunctionDeclarationKind {
        enum class Kind {
            REGULAR,
            NATIVE,
        };

        enum class Context {
            TOP_LEVEL,
            CLASS,
            INTERFACE,
        };

        enum class AccessModifier {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };

        enum class Binding {
            INSTANCE,
            STATIC,
        };
    }; // namespace FunctionDeclarationKind
} // namespace vnlc

#endif // VNLC_FUNCTION_DECLARATION_KIND_HPP