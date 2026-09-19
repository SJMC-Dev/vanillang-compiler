#ifndef VNLC_IMPORTED_ENUM_MEMBER_HPP
#define VNLC_IMPORTED_ENUM_MEMBER_HPP

#include "vni/import/ImportedEnumValue.hpp"
#include "vni/import/ImportedIdentifier.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vnlc {
    class ImportedEnumMember : public ImportedIdentifier {
    private:
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>> associatedValues;

    public:
        ImportedEnumMember(
            std::string_view name,
            std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>>&& associatedValues,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );
        ImportedEnumMember(std::string_view name, std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>>&& associatedValues);

        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>>& getAssociatedValues() const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_ENUM_MEMBER_HPP
