#ifndef VNLC_IMPORTED_ALIAS_HPP
#define VNLC_IMPORTED_ALIAS_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"

class VnlcImportedAlias : public VnlcImportedIdentifier {
private:
    std::string source;

public:
    VnlcImportedAlias(std::string_view name, std::string_view source);

    [[nodiscard]] std::string_view getSource() const;
};

#endif // VNLC_IMPORTED_ALIAS_HPP
