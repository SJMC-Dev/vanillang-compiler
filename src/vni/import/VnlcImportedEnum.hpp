#ifndef VNLC_IMPORTED_ENUM_HPP
#define VNLC_IMPORTED_ENUM_HPP

#include "vni/import/VnlcImportedEnumMember.hpp"
#include "vni/import/VnlcImportedIdentifier.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

class VnlcImportedEnum : public VnlcImportedIdentifier {
private:
    std::vector<std::string> genericParameters;
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>> members;

public:
    VnlcImportedEnum(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>>&& members,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    );

    VnlcImportedEnum(std::string_view name, std::vector<std::string>&& genericParameters, std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>>&& members);

    [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>>& getMembers() const;

    [[nodiscard]] const VnlcImportedEnumMember* getMemberByName(std::string_view name) const;
};

#endif // VNLC_IMPORTED_ENUM_HPP
