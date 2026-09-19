#ifndef VNLC_IMPORTED_PARAMETER_HPP
#define VNLC_IMPORTED_PARAMETER_HPP

#include "vni/import/ImportedIdentifier.hpp"

namespace vnlc {
    class ImportedParameter : public ImportedIdentifier {
    private:
        std::string type;

    public:
        ImportedParameter(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata);
        ImportedParameter(std::string_view name, std::string_view type);

        [[nodiscard]] std::string_view getType() const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_PARAMETER_HPP
