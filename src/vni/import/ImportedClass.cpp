#include "ImportedClass.hpp"

namespace vnlc {
    ImportedClass::ImportedClass(
        std::string_view name,
        std::optional<std::string>&& baseClass,
        std::vector<std::string>&& implementedInterfaces,
        bool final,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>&& properties,
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          final(final),
          genericParameters(std::move(genericParameters)),
          properties(std::move(properties)),
          methods(std::move(methods)) {}

    ImportedClass::ImportedClass(
        std::string_view name,
        std::optional<std::string>&& baseClass,
        std::vector<std::string>&& implementedInterfaces,
        bool final,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>&& properties,
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods
    )
        : ImportedIdentifier(name),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          final(final),
          genericParameters(std::move(genericParameters)),
          properties(std::move(properties)),
          methods(std::move(methods)) {}

    const std::optional<std::string>& ImportedClass::getBaseClass() const {
        return baseClass;
    }

    const std::vector<std::string>& ImportedClass::getImplementedInterfaces() const {
        return implementedInterfaces;
    }

    bool ImportedClass::isFinal() const {
        return final;
    }

    const std::vector<std::string>& ImportedClass::getGenericParameters() const {
        return genericParameters;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>& ImportedClass::getProperties() const {
        return properties;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>& ImportedClass::getMethods() const {
        return methods;
    }

    const ImportedProperty* ImportedClass::getPropertyByName(std::string_view name) const {
        auto it = properties.find(std::string(name));
        if (it != properties.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const ImportedMethod* ImportedClass::getMethodByName(std::string_view name) const {
        auto it = methods.find(std::string(name));
        if (it != methods.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
