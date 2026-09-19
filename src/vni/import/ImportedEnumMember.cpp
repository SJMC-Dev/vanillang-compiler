#include "ImportedEnumMember.hpp"

namespace vnlc {
    ImportedEnumMember::ImportedEnumMember(
        std::string_view name,
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>>&& associatedValues,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          associatedValues(std::move(associatedValues)) {}

    ImportedEnumMember::ImportedEnumMember(std::string_view name, std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>>&& associatedValues)
        : ImportedIdentifier(name),
          associatedValues(std::move(associatedValues)) {}

    const std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>>& ImportedEnumMember::getAssociatedValues() const {
        return associatedValues;
    }
} // namespace vnlc
