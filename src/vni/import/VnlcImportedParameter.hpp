#ifndef VNLC_IMPORTED_PARAMETER_HPP
#define VNLC_IMPORTED_PARAMETER_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"

class VnlcImportedParameter : public VnlcImportedIdentifier {
private:
    std::string type;

public:
    VnlcImportedParameter(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
    VnlcImportedParameter(std::string_view name, std::string_view type);

    [[nodiscard]] std::string_view getType() const;
};

#endif // VNLC_IMPORTED_PARAMETER_HPP
