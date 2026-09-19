#ifndef VNLC_IMPORTED_LET_HPP
#define VNLC_IMPORTED_LET_HPP

#include "vni/import/ImportedIdentifier.hpp"

namespace vnlc {
    class ImportedLet : public ImportedIdentifier {
    private:
        std::string type;

    public:
        ImportedLet(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
        ImportedLet(std::string_view name, std::string_view type);

        [[nodiscard]] std::string_view getType() const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_LET_HPP
