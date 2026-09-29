#include "ImportedInterface.hpp"

namespace vnlc {
    ImportedInterface::ImportedInterface(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          genericParameters(std::move(genericParameters)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    ImportedInterface::ImportedInterface(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>&& methods,
        std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>&& operators
    )
        : ImportedIdentifier(name),
          genericParameters(std::move(genericParameters)),
          methods(std::move(methods)),
          operators(std::move(operators)) {}

    const std::vector<std::string>& ImportedInterface::getGenericParameters() const {
        return genericParameters;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedMethod>>& ImportedInterface::getMethods() const {
        return methods;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedOperator>>& ImportedInterface::getOperators() const {
        return operators;
    }

    const ImportedMethod* ImportedInterface::getMethodByName(std::string_view name) const {
        auto it = methods.find(std::string(name));
        if (it != methods.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const ImportedOperator* ImportedInterface::getOperatorByName(std::string_view internalName) const {
        auto it = operators.find(std::string(internalName));
        if (it != operators.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
