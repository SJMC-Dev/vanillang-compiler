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
        std::unordered_map<std::string, std::unique_ptr<ImportedConstructor>>&& constructors,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedClass(
              name,
              std::move(baseClass),
              std::move(implementedInterfaces),
              final,
              std::move(genericParameters),
              std::move(properties),
              std::move(methods),
              std::move(constructors),
              std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>{},
              std::move(metadata)
          ) {}

    ImportedClass::ImportedClass(
        std::string_view name,
        std::optional<std::string>&& baseClass,
        std::vector<std::string>&& implementedInterfaces,
        bool final,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>&& properties,
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
        std::unordered_map<std::string, std::unique_ptr<ImportedConstructor>>&& constructors,
        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          final(final),
          genericParameters(std::move(genericParameters)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)) {}

    ImportedClass::ImportedClass(
        std::string_view name,
        std::optional<std::string>&& baseClass,
        std::vector<std::string>&& implementedInterfaces,
        bool final,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>&& properties,
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
        std::unordered_map<std::string, std::unique_ptr<ImportedConstructor>>&& constructors
    )
        : ImportedClass(
              name,
              std::move(baseClass),
              std::move(implementedInterfaces),
              final,
              std::move(genericParameters),
              std::move(properties),
              std::move(methods),
              std::move(constructors),
              std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>{}
          ) {}

    ImportedClass::ImportedClass(
        std::string_view name,
        std::optional<std::string>&& baseClass,
        std::vector<std::string>&& implementedInterfaces,
        bool final,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>>&& properties,
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
        std::unordered_map<std::string, std::unique_ptr<ImportedConstructor>>&& constructors,
        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators
    )
        : ImportedIdentifier(name),
          baseClass(std::move(baseClass)),
          implementedInterfaces(std::move(implementedInterfaces)),
          final(final),
          genericParameters(std::move(genericParameters)),
          properties(std::move(properties)),
          methods(std::move(methods)),
          constructors(std::move(constructors)),
          operators(std::move(operators)) {}

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

    const std::unordered_map<std::string, std::unique_ptr<ImportedConstructor>>& ImportedClass::getConstructors() const {
        return constructors;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>& ImportedClass::getOperators() const {
        return operators;
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

    const ImportedConstructor* ImportedClass::getConstructorByName(std::string_view name) const {
        auto it = constructors.find(std::string(name));
        if (it != constructors.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const ImportedOperator* ImportedClass::getOperatorByName(std::string_view internalName) const {
        auto it = operators.find(std::string(internalName));
        if (it != operators.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
