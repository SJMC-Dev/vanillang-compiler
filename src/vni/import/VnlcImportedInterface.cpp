#include "VnlcImportedInterface.hpp"

VnlcImportedInterface::VnlcImportedInterface(
    std::string_view name,
    std::vector<std::string>&& genericParameters,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>>&& methods,
    std::unordered_map<std::string, std::optional<std::string>>&& metadata
)
    : VnlcImportedIdentifier(name, std::move(metadata)),
      genericParameters(std::move(genericParameters)),
      methods(std::move(methods)) {}

VnlcImportedInterface::VnlcImportedInterface(
    std::string_view name,
    std::vector<std::string>&& genericParameters,
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>>&& methods
)
    : VnlcImportedIdentifier(name),
      genericParameters(std::move(genericParameters)),
      methods(std::move(methods)) {}

const std::vector<std::string>& VnlcImportedInterface::getGenericParameters() const {
    return genericParameters;
}

const std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>>& VnlcImportedInterface::getMethods() const {
    return methods;
}

const VnlcImportedMethod* VnlcImportedInterface::getMethodByName(std::string_view name) const {
    auto it = methods.find(std::string(name));
    if (it != methods.end()) {
        return it->second.get();
    }
    return nullptr;
}
