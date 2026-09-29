#include "ImportedConstructor.hpp"

namespace vnlc {
    ImportedConstructor::ImportedConstructor(
        std::string_view name,
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
        std::string_view accessModifier,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          parameters(std::move(parameters)),
          accessModifier(accessModifier) {}

    ImportedConstructor::ImportedConstructor(std::string_view name, std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters, std::string_view accessModifier)
        : ImportedIdentifier(name),
          parameters(std::move(parameters)),
          accessModifier(accessModifier) {}

    const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& ImportedConstructor::getParameters() const {
        return parameters;
    }

    std::string_view ImportedConstructor::getAccessModifier() const {
        return accessModifier;
    }

    const ImportedParameter* ImportedConstructor::getParameterByName(std::string_view name) const {
        auto it = parameters.find(std::string(name));
        if (it != parameters.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
