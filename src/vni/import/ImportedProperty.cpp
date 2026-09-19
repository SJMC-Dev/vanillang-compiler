#include "ImportedProperty.hpp"

namespace vnlc {
    ImportedProperty::ImportedProperty(
        std::string_view name,
        std::string_view type,
        bool staticProperty,
        std::string_view accessModifier,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          type(type),
          staticProperty(staticProperty),
          accessModifier(accessModifier) {}

    ImportedProperty::ImportedProperty(std::string_view name, std::string_view type, bool staticProperty, std::string_view accessModifier)
        : ImportedIdentifier(name),
          type(type),
          staticProperty(staticProperty),
          accessModifier(accessModifier) {}

    std::string_view ImportedProperty::getType() const {
        return type;
    }

    bool ImportedProperty::isStatic() const {
        return staticProperty;
    }

    std::string_view ImportedProperty::getAccessModifier() const {
        return accessModifier;
    }
} // namespace vnlc
