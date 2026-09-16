#ifndef VNLC_IMPORTED_METHOD_HPP
#define VNLC_IMPORTED_METHOD_HPP

#include "vni/import/VnlcImportedIdentifier.hpp"
#include "vni/import/VnlcImportedParameter.hpp"
#include <memory>
#include <unordered_map>

class VnlcImportedMethod : public VnlcImportedIdentifier {
private:
    std::string returnType;
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>> parameters;
    bool staticMethod;
    bool native;
    std::string accessModifier;

public:
    VnlcImportedMethod(
        std::string_view name,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters,
        bool staticMethod,
        bool native,
        std::string_view accessModifier,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    );

    VnlcImportedMethod(
        std::string_view name,
        std::string_view returnType,
        std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>&& parameters,
        bool staticMethod,
        bool native,
        std::string_view accessModifier
    );

    [[nodiscard]] std::string_view getReturnType() const;
    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>>& getParameters() const;
    [[nodiscard]] bool isStatic() const;
    [[nodiscard]] bool isNative() const;
    [[nodiscard]] std::string_view getAccessModifier() const;

    [[nodiscard]] const VnlcImportedParameter* getParameterByName(std::string_view name) const;
};

#endif // VNLC_IMPORTED_METHOD_HPP
