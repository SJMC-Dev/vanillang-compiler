#ifndef VNLC_IMPORTED_IDENTIFIER_HPP
#define VNLC_IMPORTED_IDENTIFIER_HPP

#include "vni/import/VnlcImportedItem.hpp"
#include <optional>
#include <unordered_map>

class VnlcImportedIdentifier : public VnlcImportedItem {
private:
    std::unordered_map<std::string, std::optional<std::string>> metadata;

protected:
    VnlcImportedIdentifier(std::string_view name, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
    VnlcImportedIdentifier(std::string_view name);

    [[nodiscard]] const std::unordered_map<std::string, std::optional<std::string>>& getMetadata() const;
};

#endif // VNLC_IMPORTED_IDENTIFIER_HPP