#include "VnlcImportedEnum.hpp"

VnlcImportedEnum::VnlcImportedEnum(
    std::string_view name,
    std::vector<std::string>&& genericParameters,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>>&& members,
    std::unordered_map<std::string, std::optional<std::string>>&& metadata
)
    : VnlcImportedIdentifier(name, std::move(metadata)),
      genericParameters(std::move(genericParameters)),
      members(std::move(members)) {}

VnlcImportedEnum::VnlcImportedEnum(std::string_view name, std::vector<std::string>&& genericParameters, std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>>&& members)
    : VnlcImportedIdentifier(name),
      genericParameters(std::move(genericParameters)),
      members(std::move(members)) {}

const std::vector<std::string>& VnlcImportedEnum::getGenericParameters() const {
    return genericParameters;
}

const std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>>& VnlcImportedEnum::getMembers() const {
    return members;
}

const VnlcImportedEnumMember* VnlcImportedEnum::getMemberByName(std::string_view name) const {
    auto it = members.find(std::string(name));
    if (it != members.end()) {
        return it->second.get();
    }
    return nullptr;
}
