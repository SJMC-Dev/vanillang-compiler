#ifndef VNLC_PROPERTY_DECLARATION_TYPE_HPP
#define VNLC_PROPERTY_DECLARATION_TYPE_HPP

namespace vnlc {
    namespace PropertyDeclarationType {
        enum class AccessModifier {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };

        enum class Binding {
            INSTANCE,
            STATIC,
        };
    }; // namespace PropertyDeclarationType
} // namespace vnlc

#endif // VNLC_PROPERTY_DECLARATION_TYPE_HPP