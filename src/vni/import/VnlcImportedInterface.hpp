#ifndef VNLC_IMPORTED_INTERFACE_HPP
#define VNLC_IMPORTED_INTERFACE_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"
#include "vni/import/VnlcImportedMethod.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

class VnlcImportedInterface : public VnlcImportedIdentifier {
private:
    std::vector<std::string> genericParameters;
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>> methods;

public:
    VnlcImportedInterface(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>>&& methods,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    );

    VnlcImportedInterface(std::string_view name, std::vector<std::string>&& genericParameters, std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>>&& methods);

    [[nodiscard]] const std::vector<std::string>& getGenericParameters() const;
    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>>& getMethods() const;

    [[nodiscard]] const VnlcImportedMethod* getMethodByName(std::string_view name) const;
};

#endif // VNLC_IMPORTED_INTERFACE_HPP
