#ifndef VNLC_FUNCTION_DECLARATION_TYPE_HPP
#define VNLC_FUNCTION_DECLARATION_TYPE_HPP

namespace vnlc {
    namespace FunctionDeclarationType {
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
    }; // namespace FunctionDeclarationType
} // namespace vnlc

#endif // VNLC_FUNCTION_DECLARATION_TYPE_HPP