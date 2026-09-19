#include "ImportedMethod.hpp"

namespace vnlc {
    ImportedMethod::ImportedMethod(
        std::string_view name,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
        bool staticMethod,
        bool native,
        std::string_view accessModifier,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          returnType(returnType),
          parameters(std::move(parameters)),
          staticMethod(staticMethod),
          native(native),
          accessModifier(accessModifier) {}

    ImportedMethod::ImportedMethod(
        std::string_view name,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>&& parameters,
        bool staticMethod,
        bool native,
        std::string_view accessModifier
    )
        : ImportedIdentifier(name),
          returnType(returnType),
          parameters(std::move(parameters)),
          staticMethod(staticMethod),
          native(native),
          accessModifier(accessModifier) {}

    std::string_view ImportedMethod::getReturnType() const {
        return returnType;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedParameter>>& ImportedMethod::getParameters() const {
        return parameters;
    }

    bool ImportedMethod::isNative() const {
        return native;
    }

    bool ImportedMethod::isStatic() const {
        return staticMethod;
    }

    std::string_view ImportedMethod::getAccessModifier() const {
        return accessModifier;
    }

    const ImportedParameter* ImportedMethod::getParameterByName(std::string_view name) const {
        auto it = parameters.find(std::string(name));
        if (it != parameters.end()) {
            return it->second.get();
        }
        return nullptr;
    }
} // namespace vnlc
