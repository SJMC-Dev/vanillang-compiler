#ifndef VNLC_IMPORTED_IDENTIFIER_HPP
#define VNLC_IMPORTED_IDENTIFIER_HPP

#include "vni/import/ImportedItem.hpp"
#include <optional>
#include <unordered_map>

namespace vnlc {
    class ImportedIdentifier : public ImportedItem {
    private:
        std::unordered_map<std::string, std::optional<std::string>> metadata;

    protected:
        ImportedIdentifier(std::string_view name, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
        ImportedIdentifier(std::string_view name);

        [[nodiscard]] const std::unordered_map<std::string, std::optional<std::string>>& getMetadata() const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_IDENTIFIER_HPP