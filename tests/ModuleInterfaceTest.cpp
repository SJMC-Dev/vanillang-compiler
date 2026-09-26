#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/DeclarationItem.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationKind.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "config/Config.hpp"
#include "config/RunningMode.hpp"
#include "error/ModuleInterfaceReaderError.hpp"
#include "error/PackageReaderError.hpp"
#include "token/Token.hpp"
#include "type/CustomizedTypeKind.hpp"
#include "type/PrimitiveType.hpp"
#include "type/VoidType.hpp"
#include "vni/export/ModuleInterfaceFileGenerator.hpp"
#include "vni/import/ImportedAlias.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedEnumValue.hpp"
#include "vni/import/ImportedFunc.hpp"
#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedInterface.hpp"
#include "vni/import/ImportedLet.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedModule.hpp"
#include "vni/import/ImportedPackage.hpp"
#include "vni/import/ImportedParameter.hpp"
#include "vni/import/ImportedProperty.hpp"
#include "vni/import/ImportedTypeAlias.hpp"
#include "vni/import/ModuleInterfaceFileReader.hpp"
#include "vni/import/PackageReader.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace vnlc {

    namespace {

        const Token testToken(TokenKind::IDENTIFIER, "test", 1, 1, 0);
        const VoidType testVoidType;

        using TypeMap = std::unordered_map<const TypeReferenceNode*, const Type*>;
        using InferredValueTypeMap = std::unordered_map<const ValueDeclarationNode*, const Type*>;
        using InferredFunctionReturnTypeMap = std::unordered_map<const FunctionDeclarationNode*, const Type*>;

        std::unique_ptr<IdentifierNode> makeIdentifier(std::string_view name) {
            return std::make_unique<IdentifierNode>(name, testToken, testToken);
        }

        std::unique_ptr<TypeReferenceNode> makeType(std::string_view name) {
            std::vector<std::unique_ptr<IdentifierNode>> nameParts;
            nameParts.push_back(makeIdentifier(name));
            return std::make_unique<CustomizedTypeReferenceNode>(false, std::move(nameParts), std::vector<std::unique_ptr<TypeReferenceNode>>{}, testToken, testToken);
        }

        std::vector<DeclarationItem::MetadataTerm> makeMetadata() {
            std::vector<DeclarationItem::MetadataTerm> metadata;
            metadata.push_back({ makeIdentifier("since"), std::string("1.0") });
            metadata.push_back({ makeIdentifier("draft"), std::nullopt });
            return metadata;
        }

        std::unique_ptr<ValueDeclarationNode> makeValue(
            ValueDeclarationKind::Kind kind,
            ValueDeclarationKind::Context context,
            ValueDeclarationKind::AccessModifier accessModifier,
            std::string_view name,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& type = std::nullopt
        ) {
            return std::make_unique<ValueDeclarationNode>(kind, context, accessModifier, makeIdentifier(name), std::move(type), std::nullopt, testToken, testToken);
        }

        std::unique_ptr<ValueDeclarationNode> makeValueWithMetadata(
            ValueDeclarationKind::Kind kind,
            ValueDeclarationKind::Context context,
            ValueDeclarationKind::AccessModifier accessModifier,
            std::string_view name,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& type,
            std::vector<DeclarationItem::MetadataTerm>&& metadata
        ) {
            return std::make_unique<ValueDeclarationNode>(kind, context, accessModifier, makeIdentifier(name), std::move(type), std::nullopt, testToken, testToken, std::move(metadata));
        }

        std::unique_ptr<FunctionDeclarationNode> makeFunction(
            FunctionDeclarationKind::Kind kind,
            FunctionDeclarationKind::Context context,
            FunctionDeclarationKind::AccessModifier accessModifier,
            FunctionDeclarationKind::Binding binding,
            std::string_view name,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& returnType,
            std::vector<DeclarationItem::MetadataTerm>&& metadata = {}
        ) {
            return std::make_unique<FunctionDeclarationNode>(
                kind,
                context,
                accessModifier,
                binding,
                makeIdentifier(name),
                std::move(parameters),
                std::move(returnType),
                std::nullopt,
                testToken,
                testToken,
                std::move(metadata)
            );
        }

        std::unique_ptr<ClassDeclarationNode> makeClass(
            bool final,
            std::string_view name,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& baseClass,
            std::vector<std::unique_ptr<TypeReferenceNode>>&& implementedInterfaces,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<std::unique_ptr<DeclarationNode>>&& memberDeclarations,
            std::vector<DeclarationItem::MetadataTerm>&& metadata = {}
        ) {
            return std::make_unique<ClassDeclarationNode>(
                final,
                makeIdentifier(name),
                std::move(baseClass),
                std::move(implementedInterfaces),
                std::move(genericParameters),
                std::move(memberDeclarations),
                testToken,
                testToken,
                std::move(metadata)
            );
        }

        std::unique_ptr<InterfaceDeclarationNode> makeInterface(
            std::string_view name,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
            std::vector<DeclarationItem::MetadataTerm>&& metadata = {}
        ) {
            return std::make_unique<InterfaceDeclarationNode>(makeIdentifier(name), std::move(genericParameters), std::move(methodDeclarations), testToken, testToken, std::move(metadata));
        }

        std::unique_ptr<EnumDeclarationNode>
        makeEnum(std::string_view name, std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters, std::vector<std::unique_ptr<EnumMemberDeclarationNode>>&& memberDeclarations) {
            return std::make_unique<EnumDeclarationNode>(makeIdentifier(name), std::move(genericParameters), std::move(memberDeclarations), testToken, testToken);
        }

        std::unique_ptr<TypeAliasDeclarationNode>
        makeTypeAlias(std::string_view name, std::vector<std::unique_ptr<IdentifierNode>>&& genericParameters, std::unique_ptr<TypeReferenceNode>&& originalType) {
            return std::make_unique<TypeAliasDeclarationNode>(makeIdentifier(name), std::move(genericParameters), std::move(originalType), testToken, testToken);
        }

        SemanticResult makeSemanticResult(TypeMap&& typeMap = {}, InferredValueTypeMap&& inferredValueTypeMap = {}, InferredFunctionReturnTypeMap&& inferredFunctionReturnTypeMap = {}) {
            return SemanticResult({}, {}, {}, {}, {}, {}, std::move(typeMap), std::move(inferredValueTypeMap), std::move(inferredFunctionReturnTypeMap), {});
        }

        Config makeGeneratorConfig(const std::filesystem::path& testDirectory) {
            const auto packageRoot = testDirectory / "package";
            const auto inputFilePath = packageRoot / "models" / "generated.vnl";
            std::filesystem::create_directories(inputFilePath.parent_path());

            return Config{
                .mode = RunningMode::COMPILE,
                .vanillangVersion = "1.0",
                .minecraftVersion = "26.1.2",
                .packageRootPath = packageRoot,
                .inputFilePath = inputFilePath,
                .outputDirectory = std::nullopt,
                .moduleInterfaceOutputDirectory = testDirectory / "interfaces",
                .dependencyPackageRootPaths = {},
                .optimizationLevel = std::nullopt,
            };
        }

        std::filesystem::path generatedOutputPath(const Config& config) {
            const auto relativeInputPath = std::filesystem::relative(config.inputFilePath, config.packageRootPath);
            auto outputPath = config.moduleInterfaceOutputDirectory.value() / config.packageRootPath.filename() / relativeInputPath;
            outputPath.replace_extension(".vni");
            return outputPath;
        }

        nlohmann::json readJson(const std::filesystem::path& path) {
            std::ifstream input(path);
            return nlohmann::json::parse(input);
        }

        class VniTest : public testing::Test {
        protected:
            std::filesystem::path testDirectory;

            void SetUp() override {
                const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
                std::string directoryName = "vnlctest-vni-" + std::string(testInfo->test_suite_name()) + "-" + std::string(testInfo->name());
                for (auto& character : directoryName) {
                    if (character == '/') {
                        character = '_';
                    }
                }

                testDirectory = std::filesystem::temp_directory_path() / directoryName;
                std::filesystem::remove_all(testDirectory);
                std::filesystem::create_directories(testDirectory);
            }

            void TearDown() override {
                std::error_code error;
                std::filesystem::remove_all(testDirectory, error);
            }

            [[nodiscard]] std::filesystem::path writeFile(const std::filesystem::path& relativePath, std::string_view contents) const {
                const auto path = testDirectory / relativePath;
                std::filesystem::create_directories(path.parent_path());
                std::ofstream output(path);
                output << contents;
                return path;
            }
        };

        class ImportedLetMetadataProbe : public ImportedLet {
        public:
            using ImportedLet::ImportedLet;

            [[nodiscard]] const std::unordered_map<std::string, std::optional<std::string>>& getMetadataProbe() const {
                return getMetadata();
            }
        };

    } // namespace

    TEST_F(VniTest, ImportedItemExposesName) {
        const ImportedLet importedLet("value", "int");

        EXPECT_EQ(importedLet.getName(), "value");
    }

    TEST_F(VniTest, ImportedLetStoresNameAndType) {
        const ImportedLet importedLet("value", "string");

        EXPECT_EQ(importedLet.getName(), "value");
        EXPECT_EQ(importedLet.getType(), "string");
    }

    TEST_F(VniTest, ImportedLetStoresMetadata) {
        std::unordered_map<std::string, std::optional<std::string>> metadata;
        metadata.emplace("since", "1.0");
        metadata.emplace("draft", std::nullopt);

        const ImportedLetMetadataProbe importedLet("value", "int", std::move(metadata));
        const auto& storedMetadata = importedLet.getMetadataProbe();

        ASSERT_EQ(storedMetadata.size(), 2);
        ASSERT_TRUE(storedMetadata.at("since").has_value());
        EXPECT_EQ(storedMetadata.at("since").value(), "1.0");
        EXPECT_FALSE(storedMetadata.at("draft").has_value());
    }

    TEST_F(VniTest, ImportedParameterStoresNameAndType) {
        const ImportedParameter parameter("amount", "int");

        EXPECT_EQ(parameter.getName(), "amount");
        EXPECT_EQ(parameter.getType(), "int");
    }

    TEST_F(VniTest, ImportedEnumValueStoresNameAndType) {
        const ImportedEnumValue enumValue("code", "int");

        EXPECT_EQ(enumValue.getName(), "code");
        EXPECT_EQ(enumValue.getType(), "int");
    }

    TEST_F(VniTest, ImportedPropertyStoresTypeStaticFlagAndAccessModifier) {
        const ImportedProperty property("count", "int", true, "protected");

        EXPECT_EQ(property.getName(), "count");
        EXPECT_EQ(property.getType(), "int");
        EXPECT_TRUE(property.isStatic());
        EXPECT_EQ(property.getAccessModifier(), "protected");
    }

    TEST_F(VniTest, ImportedAliasStoresSource) {
        const ImportedAlias alias("External", "dependency.module.External");

        EXPECT_EQ(alias.getName(), "External");
        EXPECT_EQ(alias.getSource(), "dependency.module.External");
    }

    TEST_F(VniTest, ImportedTypeAliasStoresGenericParametersAndOriginalType) {
        const ImportedTypeAlias typeAlias("Wrapper", { "T" }, "List<T>");

        EXPECT_EQ(typeAlias.getName(), "Wrapper");
        EXPECT_EQ(typeAlias.getGenericParameters(), std::vector<std::string>({ "T" }));
        EXPECT_EQ(typeAlias.getOriginalType(), "List<T>");
    }

    TEST_F(VniTest, ImportedFuncStoresReturnTypeNativeFlagAndParameters) {
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        parameters.emplace("amount", std::make_unique<ImportedParameter>("amount", "int"));

        const ImportedFunc function("add", "int", std::move(parameters), true);

        EXPECT_EQ(function.getName(), "add");
        EXPECT_EQ(function.getReturnType(), "int");
        EXPECT_TRUE(function.isNative());
        ASSERT_EQ(function.getParameters().size(), 1);
        const auto* parameter = function.getParameterByName("amount");
        ASSERT_NE(parameter, nullptr);
        EXPECT_EQ(parameter->getType(), "int");
        EXPECT_EQ(function.getParameterByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedMethodDistinguishesStaticFromNative) {
        const ImportedMethod method("run", "void", {}, true, false, "public");

        EXPECT_TRUE(method.isStatic());
        EXPECT_FALSE(method.isNative());
    }

    TEST_F(VniTest, ImportedClassStoresBaseClassInterfacesAndMembers) {
        std::unordered_map<std::string, std::unique_ptr<ImportedProperty>> properties;
        properties.emplace("count", std::make_unique<ImportedProperty>("count", "int", true, "protected"));

        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        methods.emplace("read", std::make_unique<ImportedMethod>("read", "string", std::move(parameters), false, false, "public"));

        const ImportedClass importedClass("Box", std::optional<std::string>("Base"), { "Readable" }, true, { "T" }, std::move(properties), std::move(methods));

        EXPECT_EQ(importedClass.getName(), "Box");
        ASSERT_TRUE(importedClass.getBaseClass().has_value());
        EXPECT_EQ(importedClass.getBaseClass().value(), "Base");
        EXPECT_EQ(importedClass.getImplementedInterfaces(), std::vector<std::string>({ "Readable" }));
        EXPECT_TRUE(importedClass.isFinal());
        EXPECT_EQ(importedClass.getGenericParameters(), std::vector<std::string>({ "T" }));

        const auto* property = importedClass.getPropertyByName("count");
        ASSERT_NE(property, nullptr);
        EXPECT_EQ(property->getType(), "int");
        EXPECT_TRUE(property->isStatic());

        const auto* method = importedClass.getMethodByName("read");
        ASSERT_NE(method, nullptr);
        EXPECT_EQ(method->getReturnType(), "string");
        EXPECT_FALSE(method->isStatic());
        EXPECT_FALSE(method->isNative());
    }

    TEST_F(VniTest, ImportedClassAllowsNoBaseClassAndEmptyMembers) {
        const ImportedClass importedClass("Empty", std::nullopt, {}, false, {}, {}, {});

        EXPECT_FALSE(importedClass.getBaseClass().has_value());
        EXPECT_TRUE(importedClass.getImplementedInterfaces().empty());
        EXPECT_FALSE(importedClass.isFinal());
        EXPECT_TRUE(importedClass.getGenericParameters().empty());
        EXPECT_TRUE(importedClass.getProperties().empty());
        EXPECT_TRUE(importedClass.getMethods().empty());
    }

    TEST_F(VniTest, ImportedInterfaceStoresGenericParametersAndMethods) {
        std::unordered_map<std::string, std::unique_ptr<ImportedMethod>> methods;
        std::unordered_map<std::string, std::unique_ptr<ImportedParameter>> parameters;
        methods.emplace("read", std::make_unique<ImportedMethod>("read", "string", std::move(parameters), false, false, "public"));

        const ImportedInterface importedInterface("Readable", { "T" }, std::move(methods));

        EXPECT_EQ(importedInterface.getGenericParameters(), std::vector<std::string>({ "T" }));
        const auto* method = importedInterface.getMethodByName("read");
        ASSERT_NE(method, nullptr);
        EXPECT_EQ(method->getReturnType(), "string");
    }

    TEST_F(VniTest, ImportedEnumStoresGenericParametersAndMembers) {
        std::unordered_map<std::string, std::unique_ptr<ImportedEnumValue>> associatedValues;
        associatedValues.emplace("code", std::make_unique<ImportedEnumValue>("code", "int"));

        std::unordered_map<std::string, std::unique_ptr<ImportedEnumMember>> members;
        members.emplace("Ready", std::make_unique<ImportedEnumMember>("Ready", std::move(associatedValues)));

        const ImportedEnum importedEnum("State", { "T" }, std::move(members));

        EXPECT_EQ(importedEnum.getGenericParameters(), std::vector<std::string>({ "T" }));
        const auto* member = importedEnum.getMemberByName("Ready");
        ASSERT_NE(member, nullptr);
        ASSERT_EQ(member->getAssociatedValues().size(), 1);
        EXPECT_EQ(member->getAssociatedValues().at("code")->getType(), "int");
    }

    TEST_F(VniTest, ImportedModuleExposesNameAndIdentifiers) {
        std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>> identifiers;
        identifiers.emplace("value", std::make_unique<ImportedLet>("value", "int"));

        const ImportedModule module("module", std::move(identifiers));

        EXPECT_EQ(module.getName(), "module");
        const auto* identifier = module.getIdentifierByName("value");
        ASSERT_NE(identifier, nullptr);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(identifier)->getType(), "int");
        EXPECT_EQ(module.getIdentifierByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedModuleDoesNotOverwriteExistingIdentifier) {
        ImportedModule module("module", {});

        module.addIdentifier(std::make_unique<ImportedLet>("value", "int"));
        module.addIdentifier(std::make_unique<ImportedLet>("value", "string"));

        ASSERT_EQ(module.getIdentifiers().size(), 1);
        const auto* identifier = module.getIdentifierByName("value");
        ASSERT_NE(identifier, nullptr);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(identifier)->getType(), "int");
    }

    TEST_F(VniTest, ImportedPackageExposesSubPackagesAndModules) {
        std::unordered_map<std::string, std::unique_ptr<ImportedModule>> modules;
        modules.emplace("api", std::make_unique<ImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>{}));

        const ImportedPackage package("package", {}, std::move(modules));

        const auto* module = package.getModuleByName("api");
        ASSERT_NE(module, nullptr);
        EXPECT_EQ(module->getName(), "api");
        EXPECT_EQ(package.getModuleByName("missing"), nullptr);
        EXPECT_EQ(package.getSubPackageByName("missing"), nullptr);
    }

    TEST_F(VniTest, ImportedPackageDoesNotOverwriteExistingModule) {
        ImportedPackage package("package", {}, {});

        package.addModule(std::make_unique<ImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>{}));
        package.addModule(std::make_unique<ImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<ImportedIdentifier>>{}));

        ASSERT_EQ(package.getModules().size(), 1);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderReadsModuleInterfaceFile) {
        const auto filePath = writeFile(
            "dependency/package/api.vni",
            R"({
    "value": {
        "category": "let",
        "type": "int",
        "metadata": {
            "since": "1.0",
            "draft": null
        }
    },
    "run": {
        "category": "func",
        "returnType": "void",
        "parameters": {
            "amount": {
                "category": "parameter",
                "type": "int"
            }
        },
        "native": true
    },
    "standaloneMethod": {
        "category": "method",
        "returnType": "string",
        "parameters": {},
        "static": true,
        "native": false,
        "accessModifier": "public"
    },
    "Box": {
        "category": "class",
        "genericParameters": ["T"],
        "properties": {
            "count": {
                "category": "property",
                "type": "int",
                "static": true,
                "accessModifier": "protected"
            }
        },
        "methods": {
            "read": {
                "category": "method",
                "returnType": "T",
                "parameters": {
                    "index": {
                        "category": "parameter",
                        "type": "int"
                    }
                },
                "static": false,
                "native": false,
                "accessModifier": "public"
            }
        },
        "baseClass": "Base",
        "implementedInterfaces": ["Readable"],
        "final": true
    },
    "Readable": {
        "category": "interface",
        "genericParameters": ["T"],
        "methods": {
            "read": {
                "category": "method",
                "returnType": "T",
                "parameters": {},
                "static": false,
                "native": false,
                "accessModifier": "public"
            }
        }
    },
    "State": {
        "category": "enum",
        "genericParameters": [],
        "members": {
            "Ready": {
                "category": "enummember",
                "associatedValues": {
                    "code": {
                        "category": "enumvalue",
                        "type": "int"
                    }
                }
            }
        }
    },
    "Alias": {
        "category": "typealias",
        "genericParameters": ["T"],
        "originalType": "List<T>"
    },
    "External": {
        "category": "imported",
        "source": "dependency.module.External"
    },
    "standaloneProperty": {
        "category": "property",
        "type": "string",
        "static": false,
        "accessModifier": "private"
    },
    "standaloneParameter": {
        "category": "parameter",
        "type": "string"
    }
})"
        );

        ModuleInterfaceFileReader reader(filePath);
        const auto module = reader.read();

        ASSERT_EQ(module->getName(), "api");
        ASSERT_EQ(module->getIdentifiers().size(), 10);

        const auto* value = module->getIdentifierByName("value");
        ASSERT_NE(value, nullptr);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(value)->getType(), "int");

        const auto* function = module->getIdentifierByName("run");
        ASSERT_NE(function, nullptr);
        const auto* importedFunction = dynamic_cast<const ImportedFunc*>(function);
        ASSERT_NE(importedFunction, nullptr);
        EXPECT_EQ(importedFunction->getReturnType(), "void");
        EXPECT_TRUE(importedFunction->isNative());
        const auto* parameter = importedFunction->getParameterByName("amount");
        ASSERT_NE(parameter, nullptr);
        EXPECT_EQ(parameter->getType(), "int");

        const auto* importedClassIdentifier = module->getIdentifierByName("Box");
        ASSERT_NE(importedClassIdentifier, nullptr);
        const auto* importedClass = dynamic_cast<const ImportedClass*>(importedClassIdentifier);
        ASSERT_NE(importedClass, nullptr);
        ASSERT_TRUE(importedClass->getBaseClass().has_value());
        EXPECT_EQ(importedClass->getBaseClass().value(), "Base");
        EXPECT_EQ(importedClass->getImplementedInterfaces(), std::vector<std::string>({ "Readable" }));
        EXPECT_TRUE(importedClass->isFinal());
        const auto* property = importedClass->getPropertyByName("count");
        ASSERT_NE(property, nullptr);
        EXPECT_TRUE(property->isStatic());
        EXPECT_EQ(property->getAccessModifier(), "protected");
        const auto* method = importedClass->getMethodByName("read");
        ASSERT_NE(method, nullptr);
        EXPECT_EQ(method->getReturnType(), "T");
        EXPECT_FALSE(method->isStatic());
        EXPECT_FALSE(method->isNative());

        const auto* importedInterfaceIdentifier = module->getIdentifierByName("Readable");
        ASSERT_NE(importedInterfaceIdentifier, nullptr);
        const auto* importedInterface = dynamic_cast<const ImportedInterface*>(importedInterfaceIdentifier);
        ASSERT_NE(importedInterface, nullptr);
        EXPECT_EQ(importedInterface->getGenericParameters(), std::vector<std::string>({ "T" }));

        const auto* importedEnumIdentifier = module->getIdentifierByName("State");
        ASSERT_NE(importedEnumIdentifier, nullptr);
        const auto* importedEnum = dynamic_cast<const ImportedEnum*>(importedEnumIdentifier);
        ASSERT_NE(importedEnum, nullptr);
        const auto* enumMember = importedEnum->getMemberByName("Ready");
        ASSERT_NE(enumMember, nullptr);
        ASSERT_EQ(enumMember->getAssociatedValues().size(), 1);
        EXPECT_EQ(enumMember->getAssociatedValues().at("code")->getType(), "int");

        const auto* typeAliasIdentifier = module->getIdentifierByName("Alias");
        ASSERT_NE(typeAliasIdentifier, nullptr);
        const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(typeAliasIdentifier);
        ASSERT_NE(typeAlias, nullptr);
        EXPECT_EQ(typeAlias->getOriginalType(), "List<T>");

        const auto* importedAliasIdentifier = module->getIdentifierByName("External");
        ASSERT_NE(importedAliasIdentifier, nullptr);
        const auto* importedAlias = dynamic_cast<const ImportedAlias*>(importedAliasIdentifier);
        ASSERT_NE(importedAlias, nullptr);
        EXPECT_EQ(importedAlias->getSource(), "dependency.module.External");
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnEmptyFile) {
        const auto filePath = writeFile("empty.vni", "");
        ModuleInterfaceFileReader reader(filePath);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnDirectory) {
        const auto directory = testDirectory / "directory.vni";
        std::filesystem::create_directories(directory);
        ModuleInterfaceFileReader reader(directory);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnWrongExtension) {
        const auto filePath = writeFile("module.vnl", "{}");
        ModuleInterfaceFileReader reader(filePath);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnInvalidJson) {
        const auto filePath = writeFile("module.vni", "{");
        ModuleInterfaceFileReader reader(filePath);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnUnknownCategory) {
        const auto filePath = writeFile(
            "module.vni",
            R"({
    "value": {
        "category": "unknown"
    }
})"
        );
        ModuleInterfaceFileReader reader(filePath);

        EXPECT_THROW(static_cast<void>(reader.read()), ModuleInterfaceFileReaderError);
    }

    TEST_F(VniTest, ModuleInterfaceFileReaderThrowsOnNonexistentFile) {
        EXPECT_THROW(ModuleInterfaceFileReader(testDirectory / "missing.vni"), std::filesystem::filesystem_error);
    }

    TEST_F(VniTest, PackageReaderThrowsOnMissingPackage) {
        const auto dependencyRoot = testDirectory / "dependencies";
        std::filesystem::create_directories(dependencyRoot / "available");

        Config config{};
        config.dependencyPackageRootPaths.emplace("available", dependencyRoot / "available");

        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> packages;
        PackageReader reader(packages);

        EXPECT_THROW(reader.readPackageFromPath("missing.module", config), PackageReaderError);
    }

    TEST_F(VniTest, PackageReaderReadsModuleInsidePackage) {
        const auto packageRoot = testDirectory / "dependencies" / "package";
        const auto modulePath = packageRoot / "api.vni";
        std::filesystem::create_directories(packageRoot);
        {
            std::ofstream output(modulePath);
            output << R"({
    "value": {
        "category": "let",
        "type": "int"
    }
})";
        }

        Config config{};
        config.dependencyPackageRootPaths.emplace("package", packageRoot);

        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> packages;
        PackageReader reader(packages);
        reader.readPackageFromPath("package.api", config);

        const auto package = packages.find("package");
        ASSERT_NE(package, packages.end());
        const auto* module = package->second->getModuleByName("api");
        ASSERT_NE(module, nullptr);
        ASSERT_EQ(module->getIdentifiers().size(), 1);
        EXPECT_EQ(dynamic_cast<const ImportedLet*>(module->getIdentifiers().at("value").get())->getType(), "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesLetCategory) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto type = makeType("int");
        const auto* typeNode = type.get();
        auto declaration = makeValueWithMetadata(
            ValueDeclarationKind::Kind::LET,
            ValueDeclarationKind::Context::TOP_LEVEL,
            ValueDeclarationKind::AccessModifier::PUBLIC,
            "count",
            std::make_optional(std::move(type)),
            makeMetadata()
        );
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult({ { typeNode, PrimitiveType::intType() } });

        ModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["count"]["category"], "let");
        EXPECT_EQ(json["count"]["type"], "int");
        EXPECT_EQ(json["count"]["metadata"]["since"], "1.0");
        EXPECT_TRUE(json["count"]["metadata"]["draft"].is_null());
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesNativeFuncWithParameter) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto parameterType = makeType("int");
        const auto* parameterTypeReferenceNode = parameterType.get();
        auto returnType = makeType("void");
        const auto* returnTypeReferenceNode = returnType.get();

        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        parameters.push_back(makeValue(
            ValueDeclarationKind::Kind::PARAMETER,
            ValueDeclarationKind::Context::FUNCTION,
            ValueDeclarationKind::AccessModifier::PUBLIC,
            "amount",
            std::make_optional(std::move(parameterType))
        ));
        auto declaration = makeFunction(
            FunctionDeclarationKind::Kind::NATIVE,
            FunctionDeclarationKind::Context::TOP_LEVEL,
            FunctionDeclarationKind::AccessModifier::PUBLIC,
            FunctionDeclarationKind::Binding::INSTANCE,
            "add",
            std::move(parameters),
            std::make_optional(std::move(returnType))
        );
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {
                { parameterTypeReferenceNode, PrimitiveType::intType() },
                { returnTypeReferenceNode, &testVoidType },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["add"]["category"], "func");
        EXPECT_EQ(json["add"]["returnType"], "void");
        EXPECT_TRUE(json["add"]["native"]);
        EXPECT_EQ(json["add"]["parameters"]["amount"]["category"], "parameter");
        EXPECT_EQ(json["add"]["parameters"]["amount"]["type"], "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesClassWithMembersAndModifiers) {
        const auto config = makeGeneratorConfig(testDirectory);
        CustomizedType baseType(CustomizedTypeKind::CLASS, "package.Base", std::vector<const Type*>{}, static_cast<const TypeDeclarationNode*>(nullptr));
        CustomizedType interfaceType(CustomizedTypeKind::INTERFACE, "package.Readable", std::vector<const Type*>{}, static_cast<const TypeDeclarationNode*>(nullptr));
        auto baseClass = makeType("Base");
        const auto* baseClassNode = baseClass.get();
        auto implementedInterface = makeType("Readable");
        const auto* implementedInterfaceNode = implementedInterface.get();
        auto propertyType = makeType("int");
        const auto* propertyTypeReferenceNode = propertyType.get();
        auto methodReturnType = makeType("string");
        const auto* methodReturnTypeReferenceNode = methodReturnType.get();

        std::vector<std::unique_ptr<TypeReferenceNode>> implementedInterfaces;
        implementedInterfaces.push_back(std::move(implementedInterface));

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        genericParameters.push_back(makeIdentifier("T"));

        std::vector<std::unique_ptr<DeclarationNode>> members;
        members.push_back(makeValue(
            ValueDeclarationKind::Kind::STATIC_PROPERTY,
            ValueDeclarationKind::Context::CLASS,
            ValueDeclarationKind::AccessModifier::PROTECTED,
            "count",
            std::make_optional(std::move(propertyType))
        ));
        members.push_back(makeFunction(
            FunctionDeclarationKind::Kind::NATIVE,
            FunctionDeclarationKind::Context::CLASS,
            FunctionDeclarationKind::AccessModifier::PUBLIC,
            FunctionDeclarationKind::Binding::STATIC,
            "reset",
            {},
            std::nullopt
        ));
        members.push_back(makeFunction(
            FunctionDeclarationKind::Kind::REGULAR,
            FunctionDeclarationKind::Context::CLASS,
            FunctionDeclarationKind::AccessModifier::PRIVATE,
            FunctionDeclarationKind::Binding::INSTANCE,
            "label",
            {},
            std::make_optional(std::move(methodReturnType))
        ));

        auto declaration = makeClass(true, "Box", std::make_optional(std::move(baseClass)), std::move(implementedInterfaces), std::move(genericParameters), std::move(members), makeMetadata());
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {
                { baseClassNode, &baseType },
                { implementedInterfaceNode, &interfaceType },
                { propertyTypeReferenceNode, PrimitiveType::intType() },
                { methodReturnTypeReferenceNode, PrimitiveType::stringType() },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["Box"]["category"], "class");
        EXPECT_EQ(json["Box"]["baseClass"], "package.Base");
        EXPECT_EQ(json["Box"]["implementedInterfaces"].size(), 1);
        EXPECT_EQ(json["Box"]["implementedInterfaces"][0], "package.Readable");
        EXPECT_TRUE(json["Box"]["final"]);
        EXPECT_EQ(json["Box"]["genericParameters"].size(), 1);
        EXPECT_EQ(json["Box"]["genericParameters"][0], "T");
        EXPECT_EQ(json["Box"]["metadata"]["since"], "1.0");
        EXPECT_EQ(json["Box"]["properties"]["count"]["type"], "int");
        EXPECT_TRUE(json["Box"]["properties"]["count"]["static"]);
        EXPECT_EQ(json["Box"]["properties"]["count"]["accessModifier"], "protected");
        EXPECT_TRUE(json["Box"]["methods"]["reset"]["static"]);
        EXPECT_TRUE(json["Box"]["methods"]["reset"]["native"]);
        EXPECT_FALSE(json["Box"]["methods"]["label"]["static"]);
        EXPECT_FALSE(json["Box"]["methods"]["label"]["native"]);
        EXPECT_EQ(json["Box"]["methods"]["label"]["returnType"], "string");
        EXPECT_EQ(json["Box"]["methods"]["label"]["accessModifier"], "private");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesInterfaceWithMethod) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto returnType = makeType("void");
        const auto* returnTypeReferenceNode = returnType.get();

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        genericParameters.push_back(makeIdentifier("T"));

        std::vector<std::unique_ptr<FunctionDeclarationNode>> methods;
        methods.push_back(makeFunction(
            FunctionDeclarationKind::Kind::REGULAR,
            FunctionDeclarationKind::Context::INTERFACE,
            FunctionDeclarationKind::AccessModifier::PUBLIC,
            FunctionDeclarationKind::Binding::INSTANCE,
            "run",
            {},
            std::make_optional(std::move(returnType))
        ));
        auto declaration = makeInterface("Task", std::move(genericParameters), std::move(methods));
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {
                { returnTypeReferenceNode, &testVoidType },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["Task"]["category"], "interface");
        EXPECT_EQ(json["Task"]["genericParameters"].size(), 1);
        EXPECT_EQ(json["Task"]["genericParameters"][0], "T");
        EXPECT_EQ(json["Task"]["methods"]["run"]["returnType"], "void");
        EXPECT_FALSE(json["Task"]["methods"]["run"]["native"]);
        EXPECT_FALSE(json["Task"]["methods"]["run"]["static"]);
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesEnumWithMemberAndValue) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto valueType = makeType("int");
        const auto* valueTypeReferenceNode = valueType.get();

        std::vector<std::unique_ptr<ValueDeclarationNode>> associatedValues;
        associatedValues.push_back(makeValue(
            ValueDeclarationKind::Kind::ENUM_ASSOCIATED_VALUE,
            ValueDeclarationKind::Context::ENUM_MEMBER,
            ValueDeclarationKind::AccessModifier::PUBLIC,
            "code",
            std::make_optional(std::move(valueType))
        ));

        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> members;
        members.push_back(std::make_unique<EnumMemberDeclarationNode>(makeIdentifier("Ready"), std::move(associatedValues), testToken, testToken));

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        genericParameters.push_back(makeIdentifier("T"));

        auto declaration = makeEnum("State", std::move(genericParameters), std::move(members));
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {
                { valueTypeReferenceNode, PrimitiveType::intType() },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["State"]["category"], "enum");
        EXPECT_EQ(json["State"]["genericParameters"].size(), 1);
        EXPECT_EQ(json["State"]["genericParameters"][0], "T");
        EXPECT_EQ(json["State"]["members"]["Ready"]["category"], "enummember");
        EXPECT_EQ(json["State"]["members"]["Ready"]["associatedValues"]["code"]["category"], "enumvalue");
        EXPECT_EQ(json["State"]["members"]["Ready"]["associatedValues"]["code"]["type"], "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesTypeAliasCategory) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto originalType = makeType("List<T>");
        const auto* originalTypeReferenceNode = originalType.get();

        std::vector<std::unique_ptr<IdentifierNode>> genericParameters;
        genericParameters.push_back(makeIdentifier("T"));

        auto declaration = makeTypeAlias("Wrapper", std::move(genericParameters), std::move(originalType));
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {
                { originalTypeReferenceNode, PrimitiveType::intType() },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["Wrapper"]["category"], "typealias");
        EXPECT_EQ(json["Wrapper"]["genericParameters"].size(), 1);
        EXPECT_EQ(json["Wrapper"]["genericParameters"][0], "T");
        EXPECT_EQ(json["Wrapper"]["originalType"], "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorGeneratesImportedAliasCategory) {
        const auto config = makeGeneratorConfig(testDirectory);
        const auto semantic = makeSemanticResult();

        ModuleInterfaceFileGenerator generator({}, { "External" }, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["External"]["category"], "imported");
        EXPECT_EQ(json["External"]["source"], "External");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorUsesInferredTypes) {
        const auto config = makeGeneratorConfig(testDirectory);
        auto declaration = makeValue(ValueDeclarationKind::Kind::LET, ValueDeclarationKind::Context::TOP_LEVEL, ValueDeclarationKind::AccessModifier::PUBLIC, "count", std::nullopt);
        const auto* declarationNode = declaration.get();
        const auto semantic = makeSemanticResult(
            {},
            {
                { declarationNode, PrimitiveType::intType() },
            }
        );

        ModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
        generator.generate();
        const auto json = readJson(generatedOutputPath(config));

        EXPECT_EQ(json["count"]["type"], "int");
    }

    TEST_F(VniTest, ModuleInterfaceFileGeneratorDoesNotWriteWhenOutputDirectoryIsAbsent) {
        auto config = makeGeneratorConfig(testDirectory);
        config.moduleInterfaceOutputDirectory = std::nullopt;
        const auto semantic = makeSemanticResult();

        ModuleInterfaceFileGenerator generator({}, {}, config, semantic);
        generator.generate();

        EXPECT_FALSE(std::filesystem::exists(testDirectory / "interfaces"));
    }

} // namespace vnlc
