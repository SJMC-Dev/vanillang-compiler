#ifndef VNLC_MODULE_INTERFACE_FILE_READER_HPP
#define VNLC_MODULE_INTERFACE_FILE_READER_HPP

#include "ast/declaration/VnlcImportDeclarationItem.hpp"
#include "vni/import/VnlcImportedAlias.hpp"
#include "vni/import/VnlcImportedClass.hpp"
#include "vni/import/VnlcImportedEnum.hpp"
#include "vni/import/VnlcImportedEnumMember.hpp"
#include "vni/import/VnlcImportedEnumValue.hpp"
#include "vni/import/VnlcImportedFunc.hpp"
#include "vni/import/VnlcImportedInterface.hpp"
#include "vni/import/VnlcImportedLet.hpp"
#include "vni/import/VnlcImportedMethod.hpp"
#include "vni/import/VnlcImportedModule.hpp"
#include "vni/import/VnlcImportedParameter.hpp"
#include "vni/import/VnlcImportedProperty.hpp"
#include "vni/import/VnlcImportedTypeAlias.hpp"
#include <array>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>

class VnlcModuleInterfaceFileReader {
private:
    std::filesystem::path filePath;
    const VnlcImportDeclarationItem& importItem;

    static std::array<std::string, 3> validAccessModifiers;

    std::unordered_map<std::string, std::optional<std::string>> parseImportedMetadata(const nlohmann::json& metadataJson);

    std::unique_ptr<VnlcImportedLet> parseImportedLet(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedFunc> parseImportedFunc(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedMethod> parseImportedMethod(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedClass> parseImportedClass(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedInterface> parseImportedInterface(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedEnum> parseImportedEnum(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedEnumMember> parseImportedEnumMember(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedEnumValue> parseImportedEnumValue(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedTypeAlias> parseImportedTypeAlias(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedAlias> parseImportedAlias(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedProperty> parseImportedProperty(std::string_view key, const nlohmann::json& value);
    std::unique_ptr<VnlcImportedParameter> parseImportedParameter(std::string_view key, const nlohmann::json& value);

public:
    VnlcModuleInterfaceFileReader(std::filesystem::path filePath, const VnlcImportDeclarationItem& importItem);

    [[nodiscard]] std::unique_ptr<VnlcImportedModule> read();
};

#endif // VNLC_MODULE_INTERFACE_FILE_READER_HPP