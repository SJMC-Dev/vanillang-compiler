#include "VnlcImportedFunc.hpp"

VnlcImportedFunc::VnlcImportedFunc(
    std::string_view name,
    std::string_view returnType,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters,
    bool native,
    std::unordered_map<std::string, std::optional<std::string>>&& metadata
)
    : VnlcImportedIdentifier(name, std::move(metadata)),
      returnType(returnType),
      parameters(std::move(parameters)),
      native(native) {}

VnlcImportedFunc::VnlcImportedFunc(std::string_view name, std::string_view returnType, std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters, bool native)
    : VnlcImportedIdentifier(name),
      returnType(returnType),
      parameters(std::move(parameters)),
      native(native) {}

std::string_view VnlcImportedFunc::getReturnType() const {
    return returnType;
}

const std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>& VnlcImportedFunc::getParameters() const {
    return parameters;
}

bool VnlcImportedFunc::isNative() const {
    return native;
}

const VnlcImportedParameter* VnlcImportedFunc::getParameterByName(std::string_view name) const {
    auto it = parameters.find(std::string(name));
    if (it != parameters.end()) {
        return it->second.get();
    }
    return nullptr;
}
