#ifndef VNLC_IMPORTED_FUNC_HPP
#define VNLC_IMPORTED_FUNC_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"
#include "vni/import/VnlcImportedParameter.hpp"
#include <memory>
#include <unordered_map>

class VnlcImportedFunc : public VnlcImportedIdentifier {
private:
    std::string returnType;
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>> parameters;
    bool native;

public:
    VnlcImportedFunc(
        std::string_view name,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters,
        bool native,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    );

    VnlcImportedFunc(std::string_view name, std::string_view returnType, std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters, bool native);

    [[nodiscard]] std::string_view getReturnType() const;
    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>& getParameters() const;
    [[nodiscard]] bool isNative() const;

    [[nodiscard]] std::optional<const VnlcImportedParameter*> getParameterByName(std::string_view name) const;
};

#endif // VNLC_IMPORTED_FUNC_HPP
