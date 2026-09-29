#ifndef VNLC_IMPORTED_OPERATOR_HPP
#define VNLC_IMPORTED_OPERATOR_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedParameter.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace vnlc {
    class ImportedOperator : public ImportedIdentifier {
    private:
        std::string returnType;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        std::string accessModifier;

    public:
        ImportedOperator(
            std::string_view internalName,
            std::string_view returnType,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            std::string_view accessModifier,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedOperator(
            std::string_view internalName,
            std::string_view returnType,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            std::string_view accessModifier
        );

        [[nodiscard]] std::string_view getReturnType() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& getParameters() const;
        [[nodiscard]] std::string_view getAccessModifier() const;

        [[nodiscard]] const ImportedParameter* getParameterByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_OPERATOR_HPP
