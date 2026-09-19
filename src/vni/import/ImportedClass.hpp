#ifndef VNLC_IMPORTED_CLASS_HPP
#define VNLC_IMPORTED_CLASS_HPP

#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedProperty.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class ImportedClass : public ImportedIdentifier {
    private:
        std::optional<std::string> baseClass;
        std::vector<std::string> implementedInterfaces;
        bool final;
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>> properties;
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;

    public:
        ImportedClass(
            std::string_view name,
            std::optional<std::string>&& baseClass,
            std::vector<std::string>&& implementedInterfaces,
            bool final,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>&& properties,
            std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
            std::unordered_map<std::string, std::optional<std::string>>&& metadata
        );

        ImportedClass(
            std::string_view name,
            std::optional<std::string>&& baseClass,
            std::vector<std::string>&& implementedInterfaces,
            bool final,
            std::vector<std::string>&& genericParameters,
            std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>&& properties,
            std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods
        );

        [[nodiscard]] const std::optional<std::string>& getBaseClass() const;
        [[nodiscard]] const std::vector<std::string>& getImplementedInterfaces() const;
        [[nodiscard]] bool isFinal() const;
        [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>& getProperties() const;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>& getMethods() const;

        [[nodiscard]] const ImportedProperty* getPropertyByName(std::string_view name) const;
        [[nodiscard]] const ImportedMethod* getMethodByName(std::string_view name) const;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_CLASS_HPP
