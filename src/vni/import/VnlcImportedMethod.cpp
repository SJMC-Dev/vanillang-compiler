#include "VnlcImportedMethod.hpp"

VnlcImportedMethod::VnlcImportedMethod(
    std::string_view name,
    std::string_view returnType,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters,
    bool staticMethod,
    bool native,
    std::string_view accessModifier,
    std::unordered_map<std::string, std::optional<std::string>>&& metadata
)
    : VnlcImportedIdentifier(name, std::move(metadata)),
      returnType(returnType),
      parameters(std::move(parameters)),
      staticMethod(staticMethod),
      native(native),
      accessModifier(accessModifier) {}

VnlcImportedMethod::VnlcImportedMethod(
    std::string_view name,
    std::string_view returnType,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters,
    bool staticMethod,
    bool native,
    std::string_view accessModifier
)
    : VnlcImportedIdentifier(name),
      returnType(returnType),
      parameters(std::move(parameters)),
      staticMethod(staticMethod),
      native(native),
      accessModifier(accessModifier) {}

std::string_view VnlcImportedMethod::getReturnType() const {
    return returnType;
}

const std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>& VnlcImportedMethod::getParameters() const {
    return parameters;
}

bool VnlcImportedMethod::isNative() const {
    return native;
}

bool VnlcImportedMethod::isStatic() const {
    return staticMethod;
}

std::string_view VnlcImportedMethod::getAccessModifier() const {
    return accessModifier;
}

const VnlcImportedParameter* VnlcImportedMethod::getParameterByName(std::string_view name) const {
    auto it = parameters.find(std::string(name));
    if (it != parameters.end()) {
        return it->second.get();
    }
    return nullptr;
}
