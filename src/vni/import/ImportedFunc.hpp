#ifndef VNLC_IMPORTED_FUNC_HPP
#define VNLC_IMPORTED_FUNC_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedParameter.hpp"
#include <memory>
#include <unordered_map>

namespace vnlc {
    class ImportedFunc : public ImportedIdentifier {
    private:
        std::string returnType;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        bool native;

    public:
        ImportedFunc(
            std::string_view name,
            std::string_view returnType,
            std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
            bool native,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedFunc(std::string_view name, std::string_view returnType, std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters, bool native);

        [[nodiscard]] std::string_view getReturnType() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& getParameters() const;
        [[nodiscard]] bool isNative() const;

        [[nodiscard]] const ImportedParameter* getParameterByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_FUNC_HPP
