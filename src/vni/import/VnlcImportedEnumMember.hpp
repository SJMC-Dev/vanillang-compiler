#ifndef VNLC_IMPORTED_ENUM_MEMBER_HPP
#define VNLC_IMPORTED_ENUM_MEMBER_HPP

#include "vni/import/VnlcImportedEnumValue.hpp"
#include "vni/import/VnlcImportedIdentifier.hpp"
#include <memory>
#include <string>
#include <unordered_map>

class VnlcImportedEnumMember : public VnlcImportedIdentifier {
private:
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumValue>> associatedValues;

public:
    VnlcImportedEnumMember(
        std::string_view name,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumValue>>&& associatedValues,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    );
    VnlcImportedEnumMember(std::string_view name, std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumValue>>&& associatedValues);

    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumValue>>& getAssociatedValues() const;
};

#endif // VNLC_IMPORTED_ENUM_MEMBER_HPP
