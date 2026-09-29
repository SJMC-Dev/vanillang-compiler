#ifndef VNLC_IMPORTED_CONSTRUCTOR_HPP
#define VNLC_IMPORTED_CONSTRUCTOR_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedParameter.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    class ImportedConstructor : public ImportedIdentifier {
    private:
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        std::string accessModifier;

    public:
        ImportedConstructor(
            std::string_view name,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            std::string_view accessModifier,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedConstructor(std::string_view name, std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters, std::string_view accessModifier);

        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& getParameters() const;
        [[nodiscard]] std::string_view getAccessModifier() const;

        [[nodiscard]] const ImportedParameter* getParameterByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_CONSTRUCTOR_HPP
