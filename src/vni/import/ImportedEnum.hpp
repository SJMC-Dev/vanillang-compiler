#ifndef VNLC_IMPORTED_ENUM_HPP
#define VNLC_IMPORTED_ENUM_HPP

#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedIdentifier.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class ImportedEnum : public ImportedIdentifier {
    private:
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>> members;

    public:
        ImportedEnum(
            std::string_view name,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>&& members,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedEnum(std::string_view name, std::vector<std::string>&& genericParameters, std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>&& members);

        [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>>& getMembers() const;

        [[nodiscard]] const ImportedEnumMember* getMemberByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_ENUM_HPP
