#include "ImportedEnum.hpp"

namespace vnlc {
    ImportedEnum::ImportedEnum(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>&& members,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          genericParameters(std::move(genericParameters)),
          members(std::move(members)) {}

    ImportedEnum::ImportedEnum(std::string_view name, std::vector<std::string>&& genericParameters, std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>&& members)
        : ImportedIdentifier(name),
          genericParameters(std::move(genericParameters)),
          members(std::move(members)) {}

    const std::vector<std::string>& ImportedEnum::getGenericParameters() const {
        return genericParameters;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>& ImportedEnum::getMembers() const {
        return members;
    }

    const ImportedEnumMember* ImportedEnum::getMemberByName(std::string_view name) const {
        auto it = members.find(std::string(name));
        if (it != members.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
