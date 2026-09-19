#ifndef VNLC_IMPORTED_METHOD_HPP
#define VNLC_IMPORTED_METHOD_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedParameter.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    class ImportedMethod : public ImportedIdentifier {
    private:
        std::string returnType;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        bool staticMethod;
        bool native;
        std::string accessModifier;

    public:
        ImportedMethod(
            std::string_view name,
            std::string_view returnType,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            bool staticMethod,
            bool native,
            std::string_view accessModifier,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedMethod(
            std::string_view name,
            std::string_view returnType,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            bool staticMethod,
            bool native,
            std::string_view accessModifier
        );

        [[nodiscard]] std::string_view getReturnType() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& getParameters() const;
        [[nodiscard]] bool isStatic() const;
        [[nodiscard]] bool isNative() const;
        [[nodiscard]] std::string_view getAccessModifier() const;

        [[nodiscard]] const ImportedParameter* getParameterByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_METHOD_HPP
