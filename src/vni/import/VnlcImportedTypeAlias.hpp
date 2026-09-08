#ifndef VNLC_IMPORTED_TYPE_ALIAS_HPP
#define VNLC_IMPORTED_TYPE_ALIAS_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"
#include <vector>

class VnlcImportedTypeAlias : public VnlcImportedIdentifier {
private:
    std::vector<std::string> genericParameters;
    std::string originalType;

public:
    VnlcImportedTypeAlias(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::string_view originalType,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    );
    VnlcImportedTypeAlias(std::string_view name, std::vector<std::string>&& genericParameters, std::string_view originalType);

    [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
    [[nodiscard]] std::string_view getOriginalType() const;
};

#endif // VNLC_IMPORTED_TYPE_ALIAS_HPP
