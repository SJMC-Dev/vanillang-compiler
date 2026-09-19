#include "ModuleInterfaceFileReader.hpp"
#include "error/ModuleInterfaceReaderError.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <fstream>

namespace vnlc {
    ModuleInterfaceFileReader::ModuleInterfaceFileReader(std::filesystem::path filePath) : filePath(std::filesystem::canonical(filePath)), moduleName(filePath.stem().string()) {}

    std::array<std::string, 3> ModuleInterfaceFileReader::validAccessModifiers = { "public", "protected", "private" };

    std::unordered_map<std::string, std::optional<std::string>> ModuleInterfaceFileReader::parseImportedMetadata(const nlohmann::json& metadataJson) {
        std::unordered_map<std::string, std::optional<std::string>> metadata;

        if (!metadataJson.is_object()) {
            throw ModuleInterfaceFileReaderError("Invalid metadata format");
        }

        for (auto& [key, value] : metadataJson.items()) {
            if (value.is_null()) {
                metadata[key] = std::nullopt;
            } else if (value.is_string()) {
                metadata[key] = value.get<std::string>();
            } else {
                throw ModuleInterfaceFileReaderError(fmt::format("Invalid metadata value for key '{}'", key));
            }
        }

        return metadata;
    }

    std::unique_ptr<ImportedLet> ModuleInterfaceFileReader::parseImportedLet(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::string type;

        if (!value.contains("type") || !value["type"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'type' field for let '{}'", name));
        }

        type = value["type"].get<std::string>();

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedLet>(name, type, std::move(metadata));
        }

        return std::make_unique<ImportedLet>(name, type);
    }

    std::unique_ptr<ImportedFunc> ModuleInterfaceFileReader::parseImportedFunc(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::string returnType;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        bool native;

        if (!value.contains("returnType") || !value["returnType"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'returnType' field for func '{}'", name));
        }
        if (!value.contains("native") || !value["native"].is_boolean()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'native' field for func '{}'", name));
        }
        if (!value.contains("parameters") || !value["parameters"].is_object()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'parameters' field for func '{}'", name));
        }

        returnType = value["returnType"].get<std::string>();
        native = value["native"].get<bool>();

        for (auto& [key, value] : value["parameters"].items()) {
            if (!value.is_object() || value.find("category") == value.end()) {
                throw ModuleInterfaceFileReaderError("Invalid parameter format");
            }

            parameters.emplace(key, parseImportedParameter(key, value));
        }

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedFunc>(std::move(name), std::move(returnType), std::move(parameters), native, std::move(metadata));
        }

        return std::make_unique<ImportedFunc>(std::move(name), std::move(returnType), std::move(parameters), native);
    }

    std::unique_ptr<ImportedMethod> ModuleInterfaceFileReader::parseImportedMethod(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::string returnType;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        std::string accessModifier;
        bool native;
        bool staticMethod;

        if (!value.contains("returnType") || !value["returnType"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'returnType' field for method '{}'", name));
        }
        if (!value.contains("native") || !value["native"].is_boolean()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'native' field for method '{}'", name));
        }
        if (!value.contains("static") || !value["static"].is_boolean()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'static' field for method '{}'", name));
        }
        if (!value.contains("parameters") || !value["parameters"].is_object()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'parameters' field for method '{}'", name));
        }
        if (!value.contains("accessModifier") || !value["accessModifier"].is_string() ||
            std::find(validAccessModifiers.begin(), validAccessModifiers.end(), value["accessModifier"].get<std::string>()) == validAccessModifiers.end()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'accessModifier' field for method '{}'", name));
        }

        returnType = value["returnType"].get<std::string>();
        native = value["native"].get<bool>();
        staticMethod = value["static"].get<bool>();
        accessModifier = value["accessModifier"].get<std::string>();

        for (auto& [key, value] : value["parameters"].items()) {
            if (!value.is_object() || value.find("category") == value.end()) {
                throw ModuleInterfaceFileReaderError("Invalid parameter format");
            }

            parameters.emplace(key, parseImportedParameter(key, value));
        }

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedMethod>(std::move(name), std::move(returnType), std::move(parameters), staticMethod, native, accessModifier, std::move(metadata));
        }

        return std::make_unique<ImportedMethod>(std::move(name), std::move(returnType), std::move(parameters), staticMethod, native, accessModifier);
    }

    std::unique_ptr<ImportedClass> ModuleInterfaceFileReader::parseImportedClass(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>> properties;
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;
        std::optional<std::string> baseClass;
        std::vector<std::string> implementedInterfaces;
        bool final;

        if (!value.contains("genericParameters") || !value["genericParameters"].is_array()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'genericParameters' field for class '{}'", name));
        }
        if (!value.contains("properties") || !value["properties"].is_object()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'properties' field for class '{}'", name));
        }
        if (!value.contains("methods") || !value["methods"].is_object()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'methods' field for class '{}'", name));
        }
        if (!value.contains("baseClass") || !(value["baseClass"].is_string() || value["baseClass"].is_null())) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'baseClass' field for class '{}'", name));
        }
        if (!value.contains("implementedInterfaces") || !value["implementedInterfaces"].is_array()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'implementedInterfaces' field for class '{}'", name));
        }
        if (!value.contains("final") || !value["final"].is_boolean()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'final' field for class '{}'", name));
        }

        if (value["baseClass"].is_null()) {
            baseClass = std::nullopt;
        } else {
            baseClass = value["baseClass"].get<std::string>();
        }
        for (const auto& interface : value["implementedInterfaces"]) {
            if (!interface.is_string()) {
                throw ModuleInterfaceFileReaderError(fmt::format("Invalid implemented interface in 'implementedInterfaces' field for class '{}'", name));
            }
            implementedInterfaces.push_back(interface.get<std::string>());
        }
        final = value["final"].get<bool>();

        for (const auto& param : value["genericParameters"]) {
            if (!param.is_string()) {
                throw ModuleInterfaceFileReaderError(fmt::format("Invalid generic parameter in 'genericParameters' field for class '{}'", name));
            }
            genericParameters.push_back(param.get<std::string>());
        }

        for (auto& [key, value] : value["properties"].items()) {
            if (!value.is_object() || value.find("category") == value.end()) {
                throw ModuleInterfaceFileReaderError("Invalid property format");
            }

            properties.emplace(key, parseImportedProperty(key, value));
        }

        for (auto& [key, value] : value["methods"].items()) {
            if (!value.is_object() || value.find("category") == value.end()) {
                throw ModuleInterfaceFileReaderError("Invalid method format");
            }

            methods.emplace(key, parseImportedMethod(key, value));
        }

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedClass>(
                std::move(name),
                std::move(baseClass),
                std::move(implementedInterfaces),
                final,
                std::move(genericParameters),
                std::move(properties),
                std::move(methods),
                std::move(metadata)
            );
        }

        return std::make_unique<
            ImportedClass>(std::move(name), std::move(baseClass), std::move(implementedInterfaces), final, std::move(genericParameters), std::move(properties), std::move(methods));
    }

    std::unique_ptr<ImportedInterface> ModuleInterfaceFileReader::parseImportedInterface(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;

        if (!value.contains("genericParameters") || !value["genericParameters"].is_array()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'genericParameters' field for interface '{}'", name));
        }
        if (!value.contains("methods") || !value["methods"].is_object()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'methods' field for interface '{}'", name));
        }

        for (const auto& param : value["genericParameters"]) {
            if (!param.is_string()) {
                throw ModuleInterfaceFileReaderError(fmt::format("Invalid generic parameter in 'genericParameters' field for interface '{}'", name));
            }
            genericParameters.push_back(param.get<std::string>());
        }

        for (auto& [key, value] : value["methods"].items()) {
            if (!value.is_object() || value.find("category") == value.end()) {
                throw ModuleInterfaceFileReaderError("Invalid method format");
            }

            methods.emplace(key, parseImportedMethod(key, value));
        }

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedInterface>(std::move(name), std::move(genericParameters), std::move(methods), std::move(metadata));
        }

        return std::make_unique<ImportedInterface>(std::move(name), std::move(genericParameters), std::move(methods));
    }

    std::unique_ptr<ImportedEnum> ModuleInterfaceFileReader::parseImportedEnum(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::vector<std::string> genericParameters;
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>> members;

        if (!value.contains("genericParameters") || !value["genericParameters"].is_array()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'genericParameters' field for enum '{}'", name));
        }
        if (!value.contains("members") || !value["members"].is_object()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'members' field for enum '{}'", name));
        }

        for (const auto& param : value["genericParameters"]) {
            if (!param.is_string()) {
                throw ModuleInterfaceFileReaderError(fmt::format("Invalid generic parameter in 'genericParameters' field for enum '{}'", name));
            }
            genericParameters.push_back(param.get<std::string>());
        }

        for (auto& [key, value] : value["members"].items()) {
            if (!value.is_object() || value.find("category") == value.end()) {
                throw ModuleInterfaceFileReaderError("Invalid enum member format");
            }

            members.emplace(key, parseImportedEnumMember(key, value));
        }

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedEnum>(std::move(name), std::move(genericParameters), std::move(members), std::move(metadata));
        }

        return std::make_unique<ImportedEnum>(std::move(name), std::move(genericParameters), std::move(members));
    }

    std::unique_ptr<ImportedEnumMember> ModuleInterfaceFileReader::parseImportedEnumMember(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>> associatedValues;

        if (!value.contains("associatedValues") || !value["associatedValues"].is_object()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'associatedValues' field for enum member '{}'", name));
        }

        for (auto& [key, value] : value["associatedValues"].items()) {
            if (!value.is_object() || value.find("category") == value.end()) {
                throw ModuleInterfaceFileReaderError("Invalid associated value format");
            }

            associatedValues.emplace(key, parseImportedEnumValue(key, value));
        }

        return std::make_unique<ImportedEnumMember>(std::move(name), std::move(associatedValues));
    }

    std::unique_ptr<ImportedEnumValue> ModuleInterfaceFileReader::parseImportedEnumValue(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::string type;

        if (!value.contains("type") || !value["type"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'type' field for enum value '{}'", name));
        }

        type = value["type"].get<std::string>();

        return std::make_unique<ImportedEnumValue>(std::move(name), std::move(type));
    }

    std::unique_ptr<ImportedTypeAlias> ModuleInterfaceFileReader::parseImportedTypeAlias(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::vector<std::string> genericParameters;
        std::string originalType;

        if (!value.contains("genericParameters") || !value["genericParameters"].is_array()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'genericParameters' field for type alias '{}'", name));
        }
        if (!value.contains("originalType") || !value["originalType"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'originalType' field for type alias '{}'", name));
        }

        for (const auto& param : value["genericParameters"]) {
            if (!param.is_string()) {
                throw ModuleInterfaceFileReaderError(fmt::format("Invalid generic parameter in 'genericParameters' field for type alias '{}'", name));
            }
            genericParameters.push_back(param.get<std::string>());
        }

        originalType = value["originalType"].get<std::string>();

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedTypeAlias>(std::move(name), std::move(genericParameters), std::move(originalType), std::move(metadata));
        }

        return std::make_unique<ImportedTypeAlias>(std::move(name), std::move(genericParameters), std::move(originalType));
    }

    std::unique_ptr<ImportedAlias> ModuleInterfaceFileReader::parseImportedAlias(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::string source;

        if (!value.contains("source") || !value["source"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'source' field for alias '{}'", name));
        }

        source = value["source"].get<std::string>();

        return std::make_unique<ImportedAlias>(std::move(name), std::move(source));
    }

    std::unique_ptr<ImportedProperty> ModuleInterfaceFileReader::parseImportedProperty(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::string type;
        std::string accessModifier;
        bool staticProperty;

        if (!value.contains("type") || !value["type"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'type' field for property '{}'", name));
        }
        if (!value.contains("accessModifier") || !value["accessModifier"].is_string() ||
            std::find(validAccessModifiers.begin(), validAccessModifiers.end(), value["accessModifier"].get<std::string>()) == validAccessModifiers.end()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'accessModifier' field for property '{}'", name));
        }
        if (!value.contains("static") || !value["static"].is_boolean()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'static' field for property '{}'", name));
        }

        type = value["type"].get<std::string>();
        accessModifier = value["accessModifier"].get<std::string>();
        staticProperty = value["static"].get<bool>();

        if (value.contains("metadata")) {
            std::unordered_map<std::string, std::optional<std::string>> metadata = parseImportedMetadata(value["metadata"]);
            return std::make_unique<ImportedProperty>(std::move(name), std::move(type), staticProperty, accessModifier, std::move(metadata));
        }

        return std::make_unique<ImportedProperty>(std::move(name), std::move(type), staticProperty, accessModifier);
    }

    std::unique_ptr<ImportedParameter> ModuleInterfaceFileReader::parseImportedParameter(std::string_view key, const nlohmann::json& value) {
        std::string name = std::string(key);
        std::string type;

        if (!value.contains("type") || !value["type"].is_string()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Missing or invalid 'type' field for parameter '{}'", name));
        }

        type = value["type"].get<std::string>();

        return std::make_unique<ImportedParameter>(std::move(name), std::move(type));
    }

    std::unique_ptr<ImportedModule> ModuleInterfaceFileReader::read() {
        if (!std::filesystem::exists(filePath)) {
            throw ModuleInterfaceFileReaderError(fmt::format("File {} does not exist", filePath.string()));
        } else if (!std::filesystem::is_regular_file(filePath)) {
            throw ModuleInterfaceFileReaderError(fmt::format("Path {} is not a regular file", filePath.string()));
        } else if (std::filesystem::is_empty(filePath)) {
            throw ModuleInterfaceFileReaderError(fmt::format("File {} is empty", filePath.string()));
        } else if (std::filesystem::is_directory(filePath)) {
            throw ModuleInterfaceFileReaderError(fmt::format("Path {} is a directory, not a file", filePath.string()));
        } else if (!std::filesystem::is_regular_file(filePath)) {
            throw ModuleInterfaceFileReaderError(fmt::format("Path {} is not a regular file", filePath.string()));
        } else if (filePath.extension() != ".vni") {
            throw ModuleInterfaceFileReaderError(fmt::format("File {} is not a module interface file", filePath.string()));
        }

        nlohmann::json json;
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw ModuleInterfaceFileReaderError(fmt::format("Could not open module interface file {}", filePath.string()));
        }

        try {
            json = nlohmann::json::parse(file);
        } catch (const nlohmann::json::parse_error& e) {
            throw ModuleInterfaceFileReaderError(e.what());
        }

        if (!json.is_object()) {
            throw ModuleInterfaceFileReaderError("Invalid module interface file");
        }

        std::unique_ptr<ImportedModule> importedModule = std::make_unique<ImportedModule>(moduleName, std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>());

        for (auto& [key, value] : json.items()) {
            if (!value.is_object() || value.find("category") == value.end() || !value["category"].is_string()) {
                throw ModuleInterfaceFileReaderError("Invalid module interface file");
            }

            if (value["category"] == "let") {
                importedModule->addIdentifier(parseImportedLet(key, value));
            } else if (value["category"] == "func") {
                importedModule->addIdentifier(parseImportedFunc(key, value));
            } else if (value["category"] == "method") {
                importedModule->addIdentifier(parseImportedMethod(key, value));
            } else if (value["category"] == "class") {
                importedModule->addIdentifier(parseImportedClass(key, value));
            } else if (value["category"] == "interface") {
                importedModule->addIdentifier(parseImportedInterface(key, value));
            } else if (value["category"] == "enum") {
                importedModule->addIdentifier(parseImportedEnum(key, value));
            } else if (value["category"] == "enummember") {
                importedModule->addIdentifier(parseImportedEnumMember(key, value));
            } else if (value["category"] == "typealias") {
                importedModule->addIdentifier(parseImportedTypeAlias(key, value));
            } else if (value["category"] == "imported") {
                importedModule->addIdentifier(parseImportedAlias(key, value));
            } else if (value["category"] == "property") {
                importedModule->addIdentifier(parseImportedProperty(key, value));
            } else if (value["category"] == "parameter") {
                importedModule->addIdentifier(parseImportedParameter(key, value));
            } else {
                throw ModuleInterfaceFileReaderError(fmt::format("Unknown category: {}", value["category"].get<std::string>()));
            }
        }

        return importedModule;
    }
} // namespace vnlc
