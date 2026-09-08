#ifndef VNLC_IMPORTED_ENUM_VALUE_HPP
#define VNLC_IMPORTED_ENUM_VALUE_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"
#include <string>

class VnlcImportedEnumValue : public VnlcImportedIdentifier {
private:
    std::string type;

public:
    VnlcImportedEnumValue(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
    VnlcImportedEnumValue(std::string_view name, std::string_view type);

    [[nodiscard]] std::string_view getType() const;
};

#endif // VNLC_IMPORTED_ENUM_VALUE_HPP