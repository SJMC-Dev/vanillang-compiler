#ifndef VNLC_IMPORTED_PROPERTY_HPP
#define VNLC_IMPORTED_PROPERTY_HPP

#include "vni/import/ImportedIdentifier.hpp"

namespace vnlc {
    class ImportedProperty : public ImportedIdentifier {
    private:
        std::string type;
        bool staticProperty;
        std::string accessModifier;

    public:
        ImportedProperty(
            std::string_view name,
            std::string_view type,
            bool staticProperty,
            std::string_view accessModifier,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );
        ImportedProperty(std::string_view name, std::string_view type, bool staticProperty, std::string_view accessModifier);

        [[nodiscard]] std::string_view getType() const;
        [[nodiscard]] bool isStatic() const;
        [[nodiscard]] std::string_view getAccessModifier() const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_PROPERTY_HPP
