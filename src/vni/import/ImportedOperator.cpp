#include "ImportedOperator.hpp"

namespace vnlc {
    ImportedOperator::ImportedOperator(
        std::string_view internalName,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
        std::string_view accessModifier,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(internalName, std::move(metadata)),
          returnType(returnType),
          parameters(std::move(parameters)),
          accessModifier(accessModifier) {}

    ImportedOperator::ImportedOperator(
        std::string_view internalName,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
        std::string_view accessModifier
    )
        : ImportedIdentifier(internalName),
          returnType(returnType),
          parameters(std::move(parameters)),
          accessModifier(accessModifier) {}

    std::string_view ImportedOperator::getReturnType() const {
        return returnType;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& ImportedOperator::getParameters() const {
        return parameters;
    }

    std::string_view ImportedOperator::getAccessModifier() const {
        return accessModifier;
    }

    const ImportedParameter* ImportedOperator::getParameterByName(std::string_view name) const {
        auto it = parameters.find(std::string(name));
        if (it != parameters.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
