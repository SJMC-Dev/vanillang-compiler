#ifndef VNLC_IMPORTED_ALIAS_HPP
#define VNLC_IMPORTED_ALIAS_HPP

#include "vni/import/ImportedIdentifier.hpp"

namespace vnlc {
    class ImportedAlias : public ImportedIdentifier {
    private:
        std::string source;

    public:
        ImportedAlias(std::string_view name, std::string_view source);

        [[nodiscard]] std::string_view getSource() const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_ALIAS_HPP
