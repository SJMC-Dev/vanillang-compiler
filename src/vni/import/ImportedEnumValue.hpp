#ifndef VNLC_IMPORTED_ENUM_VALUE_HPP
#define VNLC_IMPORTED_ENUM_VALUE_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include <string>

namespace vnlc {
    class ImportedEnumValue : public ImportedIdentifier {
    private:
        std::string type;

    public:
        ImportedEnumValue(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
        ImportedEnumValue(std::string_view name, std::string_view type);

        [[nodiscard]] std::string_view getType() const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_ENUM_VALUE_HPP