#ifndef VNLC_IMPORTED_LET_HPP
#define VNLC_IMPORTED_LET_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"

class VnlcImportedLet : public VnlcImportedIdentifier {
private:
    std::string type;

public:
    VnlcImportedLet(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
    VnlcImportedLet(std::string_view name, std::string_view type);

    [[nodiscard]] std::string_view getType() const;
};

#endif // VNLC_IMPORTED_LET_HPP
