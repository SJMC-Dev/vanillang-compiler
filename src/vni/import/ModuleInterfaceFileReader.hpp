#ifndef VNLC_MODULE_INTERFACE_FILE_READER_HPP
#define VNLC_MODULE_INTERFACE_FILE_READER_HPP

#include "vni/import/ImportedAlias.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedEnumValue.hpp"
#include "vni/import/ImportedFunc.hpp"
#include "vni/import/ImportedInterface.hpp"
#include "vni/import/ImportedLet.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedModule.hpp"
#include "vni/import/ImportedParameter.hpp"
#include "vni/import/ImportedProperty.hpp"
#include "vni/import/ImportedTypeAlias.hpp"
#include <array>
#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace vnlc {
    class ModuleInterfaceFileReader {
    private:
        std::filesystem::path filePath;
        std::string moduleName;

        static std::array<std::string, 3> validAccessModifiers;

        std::unordered_map<std::string, std::optional<std::string>> parseImportedMetadata(const nlohmann::json& metadataJson);

        std::unique_ptr<ImportedLet> parseImportedLet(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedFunc> parseImportedFunc(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedMethod> parseImportedMethod(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedClass> parseImportedClass(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedInterface> parseImportedInterface(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedEnum> parseImportedEnum(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedEnumMember> parseImportedEnumMember(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedEnumValue> parseImportedEnumValue(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedTypeAlias> parseImportedTypeAlias(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedAlias> parseImportedAlias(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedProperty> parseImportedProperty(std::string_view key, const nlohmann::json& value);
        std::unique_ptr<ImportedParameter> parseImportedParameter(std::string_view key, const nlohmann::json& value);

    public:
        ModuleInterfaceFileReader(std::filesystem::path filePath);

        [[nodiscard]] std::unique_ptr<ImportedModule> read();
    };
} // namespace vnlc

#endif // VNLC_MODULE_INTERFACE_FILE_READER_HPP
