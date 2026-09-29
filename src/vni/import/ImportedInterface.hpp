#ifndef VNLC_IMPORTED_INTERFACE_HPP
#define VNLC_IMPORTED_INTERFACE_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedOperator.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class ImportedInterface : public ImportedIdentifier {
    private:
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;
        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>> operators;

    public:
        ImportedInterface(
            std::string_view name,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
            std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedInterface(
            std::string_view name,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
            std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators
        );

        [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>& getMethods() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>& getOperators() const;

        [[nodiscard]] const ImportedMethod* getMethodByName(std::string_view name) const;
        [[nodiscard]] const ImportedOperator* getOperatorByName(std::string_view internalName) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_INTERFACE_HPP
