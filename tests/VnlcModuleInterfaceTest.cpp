#include "ast/declaration/VnlcClassDeclarationNode.hpp"
#include "ast/declaration/VnlcDeclarationItem.hpp"
#include "ast/declaration/VnlcEnumDeclarationNode.hpp"
#include "ast/declaration/VnlcEnumMemberDeclarationNode.hpp"
#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include "ast/declaration/VnlcFunctionDeclarationType.hpp"
#include "ast/declaration/VnlcInterfaceDeclarationNode.hpp"
#include "ast/declaration/VnlcTypeAliasDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "ast/declaration/VnlcValueDeclarationType.hpp"
#include "ast/identifier/VnlcIdentifierNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include "config/VnlcConfig.hpp"
#include "config/VnlcRunningMode.hpp"
#include "error/VnlcModuleInterfaceReaderError.hpp"
#include "error/VnlcPackageReaderError.hpp"
#include "token/VnlcToken.hpp"
#include "type/VnlcCustomizedTypeKind.hpp"
#include "type/VnlcPrimitiveType.hpp"
#include "type/VnlcVoidType.hpp"
#include "vni/export/VnlcModuleInterfaceFileGenerator.hpp"
#include "vni/import/VnlcImportedAlias.hpp"
#include "vni/import/VnlcImportedClass.hpp"
#include "vni/import/VnlcImportedEnum.hpp"
#include "vni/import/VnlcImportedEnumMember.hpp"
#include "vni/import/VnlcImportedEnumValue.hpp"
#include "vni/import/VnlcImportedFunc.hpp"
#include "vni/import/VnlcImportedIdentifier.hpp"
#include "vni/import/VnlcImportedInterface.hpp"
#include "vni/import/VnlcImportedLet.hpp"
#include "vni/import/VnlcImportedMethod.hpp"
#include "vni/import/VnlcImportedModule.hpp"
#include "vni/import/VnlcImportedPackage.hpp"
#include "vni/import/VnlcImportedParameter.hpp"
#include "vni/import/VnlcImportedProperty.hpp"
#include "vni/import/VnlcImportedTypeAlias.hpp"
#include "vni/import/VnlcModuleInterfaceFileReader.hpp"
#include "vni/import/VnlcPackageReader.hpp"
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

namespace {

    const VnlcToken testToken(VnlcTokenType::IDENTIFIER, "test", 1, 1, 0);
    const VnlcVoidType testVoidType;

    using TypeMap = std::unordered_map<const VnlcTypeNode*, const VnlcSemanticType*>;
    using InferredValueTypeMap = std::unordered_map<const VnlcValueDeclarationNode*, const VnlcSemanticType*>;
    using InferredFunctionReturnTypeMap = std::unordered_map<const VnlcFunctionDeclarationNode*, const VnlcSemanticType*>;

    std::unique_ptr<VnlcIdentifierNode> makeIdentifier(std::string_view name) {
        return std::make_unique<VnlcIdentifierNode>(name, testToken, testToken);
    }

    std::unique_ptr<VnlcTypeNode> makeType(std::string_view name) {
        std::vector<std::unique_ptr<VnlcIdentifierNode>> nameParts;
        nameParts.push_back(makeIdentifier(name));
        return std::make_unique<VnlcTypeNode>(false, std::move(nameParts), std::vector<std::unique_ptr<VnlcTypeNode>>{}, testToken, testToken);
    }

    std::vector<VnlcDeclarationItem::MetadataTerm> makeMetadata() {
        std::vector<VnlcDeclarationItem::MetadataTerm> metadata;
        metadata.push_back({ makeIdentifier("since"), std::string("1.0") });
        metadata.push_back({ makeIdentifier("draft"), std::nullopt });
        return metadata;
    }

    std::unique_ptr<VnlcValueDeclarationNode> makeValue(
        VnlcValueDeclarationType::Kind kind,
        VnlcValueDeclarationType::Context context,
        VnlcValueDeclarationType::AccessModifier accessModifier,
        std::string_view name,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& type = std::nullopt
    ) {
        return std::make_unique<VnlcValueDeclarationNode>(kind, context, accessModifier, makeIdentifier(name), std::move(type), std::nullopt, testToken, testToken);
    }

    std::unique_ptr<VnlcValueDeclarationNode> makeValueWithMetadata(
        VnlcValueDeclarationType::Kind kind,
        VnlcValueDeclarationType::Context context,
        VnlcValueDeclarationType::AccessModifier accessModifier,
        std::string_view name,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& type,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadata
    ) {
        return std::make_unique<VnlcValueDeclarationNode>(kind, context, accessModifier, makeIdentifier(name), std::move(type), std::nullopt, testToken, testToken, std::move(metadata));
    }

    std::unique_ptr<VnlcFunctionDeclarationNode> makeFunction(
        VnlcFunctionDeclarationType::Kind kind,
        VnlcFunctionDeclarationType::Context context,
        VnlcFunctionDeclarationType::AccessModifier accessModifier,
        VnlcFunctionDeclarationType::Binding binding,
        std::string_view name,
        std::vector<std::unique_ptr<VnlcValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& returnType,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadata = {}
    ) {
        return std::make_unique<VnlcFunctionDeclarationNode>(
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

    std::unique_ptr<VnlcClassDeclarationNode> makeClass(
        bool final,
        std::string_view name,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& baseClass,
        std::vector<std::unique_ptr<VnlcTypeNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameters,
        std::vector<std::unique_ptr<VnlcDeclarationNode>>&& memberDeclarations,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadata = {}
    ) {
        return std::make_unique<VnlcClassDeclarationNode>(
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

    std::unique_ptr<VnlcInterfaceDeclarationNode> makeInterface(
        std::string_view name,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameters,
        std::vector<std::unique_ptr<VnlcFunctionDeclarationNode>>&& methodDeclarations,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadata = {}
    ) {
        return std::make_unique<VnlcInterfaceDeclarationNode>(makeIdentifier(name), std::move(genericParameters), std::move(methodDeclarations), testToken, testToken, std::move(metadata));
    }

    std::unique_ptr<VnlcEnumDeclarationNode>
    makeEnum(std::string_view name, std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameters, std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>>&& memberDeclarations) {
        return std::make_unique<VnlcEnumDeclarationNode>(makeIdentifier(name), std::move(genericParameters), std::move(memberDeclarations), testToken, testToken);
    }

    std::unique_ptr<VnlcTypeAliasDeclarationNode>
    makeTypeAlias(std::string_view name, std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameters, std::unique_ptr<VnlcTypeNode>&& originalType) {
        return std::make_unique<VnlcTypeAliasDeclarationNode>(makeIdentifier(name), std::move(genericParameters), std::move(originalType), testToken, testToken);
    }

    VnlcSemanticAnalysisResult
    makeSemanticResult(TypeMap&& semanticTypeMap = {}, InferredValueTypeMap&& inferredValueTypeMap = {}, InferredFunctionReturnTypeMap&& inferredFunctionReturnTypeMap = {}) {
        return VnlcSemanticAnalysisResult({}, {}, {}, {}, {}, std::move(semanticTypeMap), std::move(inferredValueTypeMap), std::move(inferredFunctionReturnTypeMap), {}, {});
    }

    VnlcImportDeclarationItem makeImportItem(std::initializer_list<std::string_view> prefix) {
        VnlcImportDeclarationItem importItem;
        for (const auto name : prefix) {
            importItem.namePrefix.push_back(makeIdentifier(name));
        }
        return importItem;
    }

    VnlcConfig makeGeneratorConfig(const std::filesystem::path& testDirectory) {
        const auto packageRoot = testDirectory / "package";
        const auto inputFilePath = packageRoot / "models" / "generated.vnl";
        std::filesystem::create_directories(inputFilePath.parent_path());

        return VnlcConfig{
            .mode = VnlcRunningMode::COMPILE,
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

    std::filesystem::path generatedOutputPath(const VnlcConfig& config) {
        const auto relativeInputPath = std::filesystem::relative(config.inputFilePath, config.packageRootPath);
        auto outputPath = config.moduleInterfaceOutputDirectory.value() / config.packageRootPath.filename() / relativeInputPath;
        outputPath.replace_extension(".vni");
        return outputPath;
    }

    nlohmann::json readJson(const std::filesystem::path& path) {
        std::ifstream input(path);
        return nlohmann::json::parse(input);
    }

    class VnlcVniTest : public testing::Test {
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

    class VnlcImportedLetMetadataProbe : public VnlcImportedLet {
    public:
        using VnlcImportedLet::VnlcImportedLet;

        [[nodiscard]] const std::unordered_map<std::string, std::optional<std::string>>& getMetadataProbe() const {
            return getMetadata();
        }
    };

} // namespace

TEST_F(VnlcVniTest, VnlcImportedItemExposesName) {
    const VnlcImportedLet importedLet("value", "int");

    EXPECT_EQ(importedLet.getName(), "value");
}

TEST_F(VnlcVniTest, VnlcImportedLetStoresNameAndType) {
    const VnlcImportedLet importedLet("value", "string");

    EXPECT_EQ(importedLet.getName(), "value");
    EXPECT_EQ(importedLet.getType(), "string");
}

TEST_F(VnlcVniTest, VnlcImportedLetStoresMetadata) {
    std::unordered_map<std::string, std::optional<std::string>> metadata;
    metadata.emplace("since", "1.0");
    metadata.emplace("draft", std::nullopt);

    const VnlcImportedLetMetadataProbe importedLet("value", "int", std::move(metadata));
    const auto& storedMetadata = importedLet.getMetadataProbe();

    ASSERT_EQ(storedMetadata.size(), 2);
    ASSERT_TRUE(storedMetadata.at("since").has_value());
    EXPECT_EQ(storedMetadata.at("since").value(), "1.0");
    EXPECT_FALSE(storedMetadata.at("draft").has_value());
}

TEST_F(VnlcVniTest, VnlcImportedParameterStoresNameAndType) {
    const VnlcImportedParameter parameter("amount", "int");

    EXPECT_EQ(parameter.getName(), "amount");
    EXPECT_EQ(parameter.getType(), "int");
}

TEST_F(VnlcVniTest, VnlcImportedEnumValueStoresNameAndType) {
    const VnlcImportedEnumValue enumValue("code", "int");

    EXPECT_EQ(enumValue.getName(), "code");
    EXPECT_EQ(enumValue.getType(), "int");
}

TEST_F(VnlcVniTest, VnlcImportedPropertyStoresTypeStaticFlagAndAccessModifier) {
    const VnlcImportedProperty property("count", "int", true, "protected");

    EXPECT_EQ(property.getName(), "count");
    EXPECT_EQ(property.getType(), "int");
    EXPECT_TRUE(property.isStatic());
    EXPECT_EQ(property.getAccessModifier(), "protected");
}

TEST_F(VnlcVniTest, VnlcImportedAliasStoresSource) {
    const VnlcImportedAlias alias("External", "dependency.module.External");

    EXPECT_EQ(alias.getName(), "External");
    EXPECT_EQ(alias.getSource(), "dependency.module.External");
}

TEST_F(VnlcVniTest, VnlcImportedTypeAliasStoresGenericParametersAndOriginalType) {
    const VnlcImportedTypeAlias typeAlias("Wrapper", { "T" }, "List<T>");

    EXPECT_EQ(typeAlias.getName(), "Wrapper");
    EXPECT_EQ(typeAlias.getGenericParameters(), std::vector<std::string>({ "T" }));
    EXPECT_EQ(typeAlias.getOriginalType(), "List<T>");
}

TEST_F(VnlcVniTest, VnlcImportedFuncStoresReturnTypeNativeFlagAndParameters) {
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>> parameters;
    parameters.emplace("amount", std::make_unique<VnlcImportedParameter>("amount", "int"));

    const VnlcImportedFunc function("add", "int", std::move(parameters), true);

    EXPECT_EQ(function.getName(), "add");
    EXPECT_EQ(function.getReturnType(), "int");
    EXPECT_TRUE(function.isNative());
    ASSERT_EQ(function.getParameters().size(), 1);
    const auto parameter = function.getParameterByName("amount");
    ASSERT_TRUE(parameter.has_value());
    EXPECT_EQ(parameter.value()->getType(), "int");
    EXPECT_FALSE(function.getParameterByName("missing").has_value());
}

TEST_F(VnlcVniTest, VnlcImportedMethodDistinguishesStaticFromNative) {
    const VnlcImportedMethod method("run", "void", {}, true, false, "public");

    EXPECT_TRUE(method.isStatic());
    EXPECT_FALSE(method.isNative());
}

TEST_F(VnlcVniTest, VnlcImportedClassStoresBaseClassInterfacesAndMembers) {
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedProperty>> properties;
    properties.emplace("count", std::make_unique<VnlcImportedProperty>("count", "int", true, "protected"));

    std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>> methods;
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>> parameters;
    methods.emplace("read", std::make_unique<VnlcImportedMethod>("read", "string", std::move(parameters), false, false, "public"));

    const VnlcImportedClass importedClass("Box", std::optional<std::string>("Base"), { "Readable" }, true, { "T" }, std::move(properties), std::move(methods));

    EXPECT_EQ(importedClass.getName(), "Box");
    ASSERT_TRUE(importedClass.getBaseClass().has_value());
    EXPECT_EQ(importedClass.getBaseClass().value(), "Base");
    EXPECT_EQ(importedClass.getImplementedInterfaces(), std::vector<std::string>({ "Readable" }));
    EXPECT_TRUE(importedClass.isFinal());
    EXPECT_EQ(importedClass.getGenericParameters(), std::vector<std::string>({ "T" }));

    const auto property = importedClass.getPropertyByName("count");
    ASSERT_TRUE(property.has_value());
    EXPECT_EQ(property.value()->getType(), "int");
    EXPECT_TRUE(property.value()->isStatic());

    const auto method = importedClass.getMethodByName("read");
    ASSERT_TRUE(method.has_value());
    EXPECT_EQ(method.value()->getReturnType(), "string");
    EXPECT_FALSE(method.value()->isStatic());
    EXPECT_FALSE(method.value()->isNative());
}

TEST_F(VnlcVniTest, VnlcImportedClassAllowsNoBaseClassAndEmptyMembers) {
    const VnlcImportedClass importedClass("Empty", std::nullopt, {}, false, {}, {}, {});

    EXPECT_FALSE(importedClass.getBaseClass().has_value());
    EXPECT_TRUE(importedClass.getImplementedInterfaces().empty());
    EXPECT_FALSE(importedClass.isFinal());
    EXPECT_TRUE(importedClass.getGenericParameters().empty());
    EXPECT_TRUE(importedClass.getProperties().empty());
    EXPECT_TRUE(importedClass.getMethods().empty());
}

TEST_F(VnlcVniTest, VnlcImportedInterfaceStoresGenericParametersAndMethods) {
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedMethod>> methods;
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedParameter>> parameters;
    methods.emplace("read", std::make_unique<VnlcImportedMethod>("read", "string", std::move(parameters), false, false, "public"));

    const VnlcImportedInterface importedInterface("Readable", { "T" }, std::move(methods));

    EXPECT_EQ(importedInterface.getGenericParameters(), std::vector<std::string>({ "T" }));
    const auto method = importedInterface.getMethodByName("read");
    ASSERT_TRUE(method.has_value());
    EXPECT_EQ(method.value()->getReturnType(), "string");
}

TEST_F(VnlcVniTest, VnlcImportedEnumStoresGenericParametersAndMembers) {
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumValue>> associatedValues;
    associatedValues.emplace("code", std::make_unique<VnlcImportedEnumValue>("code", "int"));

    std::unordered_map<std::string, std::unique_ptr<VnlcImportedEnumMember>> members;
    members.emplace("Ready", std::make_unique<VnlcImportedEnumMember>("Ready", std::move(associatedValues)));

    const VnlcImportedEnum importedEnum("State", { "T" }, std::move(members));

    EXPECT_EQ(importedEnum.getGenericParameters(), std::vector<std::string>({ "T" }));
    const auto member = importedEnum.getMemberByName("Ready");
    ASSERT_TRUE(member.has_value());
    ASSERT_EQ(member.value()->getAssociatedValues().size(), 1);
    EXPECT_EQ(member.value()->getAssociatedValues().at("code")->getType(), "int");
}

TEST_F(VnlcVniTest, VnlcImportedModuleExposesNameAndIdentifiers) {
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>> identifiers;
    identifiers.emplace("value", std::make_unique<VnlcImportedLet>("value", "int"));

    const VnlcImportedModule module("module", std::move(identifiers));

    EXPECT_EQ(module.getName(), "module");
    const auto identifier = module.getIdentifierByName("value");
    ASSERT_TRUE(identifier.has_value());
    EXPECT_EQ(dynamic_cast<const VnlcImportedLet*>(identifier.value())->getType(), "int");
    EXPECT_FALSE(module.getIdentifierByName("missing").has_value());
}

TEST_F(VnlcVniTest, VnlcImportedModuleDoesNotOverwriteExistingIdentifier) {
    VnlcImportedModule module("module", {});

    module.addIdentifier(std::make_unique<VnlcImportedLet>("value", "int"));
    module.addIdentifier(std::make_unique<VnlcImportedLet>("value", "string"));

    ASSERT_EQ(module.getIdentifiers().size(), 1);
    const auto identifier = module.getIdentifierByName("value");
    ASSERT_TRUE(identifier.has_value());
    EXPECT_EQ(dynamic_cast<const VnlcImportedLet*>(identifier.value())->getType(), "int");
}

TEST_F(VnlcVniTest, VnlcImportedPackageExposesSubPackagesAndModules) {
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedModule>> modules;
    modules.emplace("api", std::make_unique<VnlcImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>>{}));

    const VnlcImportedPackage package("package", {}, std::move(modules));

    const auto module = package.getModuleByName("api");
    ASSERT_TRUE(module.has_value());
    EXPECT_EQ(module.value()->getName(), "api");
    EXPECT_FALSE(package.getModuleByName("missing").has_value());
    EXPECT_FALSE(package.getSubPackageByName("missing").has_value());
}

TEST_F(VnlcVniTest, VnlcImportedPackageDoesNotOverwriteExistingModule) {
    VnlcImportedPackage package("package", {}, {});

    package.addModule(std::make_unique<VnlcImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>>{}));
    package.addModule(std::make_unique<VnlcImportedModule>("api", std::unordered_map<std::string, std::unique_ptr<VnlcImportedIdentifier>>{}));

    ASSERT_EQ(package.getModules().size(), 1);
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileReaderReadsModuleInterfaceFile) {
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

    const auto importItem = makeImportItem({ "package", "api" });
    VnlcModuleInterfaceFileReader reader(filePath);
    const auto module = reader.read();

    ASSERT_EQ(module->getName(), "api");
    ASSERT_EQ(module->getIdentifiers().size(), 10);

    const auto value = module->getIdentifierByName("value");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(dynamic_cast<const VnlcImportedLet*>(value.value())->getType(), "int");

    const auto function = module->getIdentifierByName("run");
    ASSERT_TRUE(function.has_value());
    const auto* importedFunction = dynamic_cast<const VnlcImportedFunc*>(function.value());
    ASSERT_NE(importedFunction, nullptr);
    EXPECT_EQ(importedFunction->getReturnType(), "void");
    EXPECT_TRUE(importedFunction->isNative());
    const auto parameter = importedFunction->getParameterByName("amount");
    ASSERT_TRUE(parameter.has_value());
    EXPECT_EQ(parameter.value()->getType(), "int");

    const auto importedClassIdentifier = module->getIdentifierByName("Box");
    ASSERT_TRUE(importedClassIdentifier.has_value());
    const auto* importedClass = dynamic_cast<const VnlcImportedClass*>(importedClassIdentifier.value());
    ASSERT_NE(importedClass, nullptr);
    ASSERT_TRUE(importedClass->getBaseClass().has_value());
    EXPECT_EQ(importedClass->getBaseClass().value(), "Base");
    EXPECT_EQ(importedClass->getImplementedInterfaces(), std::vector<std::string>({ "Readable" }));
    EXPECT_TRUE(importedClass->isFinal());
    const auto property = importedClass->getPropertyByName("count");
    ASSERT_TRUE(property.has_value());
    EXPECT_TRUE(property.value()->isStatic());
    EXPECT_EQ(property.value()->getAccessModifier(), "protected");
    const auto method = importedClass->getMethodByName("read");
    ASSERT_TRUE(method.has_value());
    EXPECT_EQ(method.value()->getReturnType(), "T");
    EXPECT_FALSE(method.value()->isStatic());
    EXPECT_FALSE(method.value()->isNative());

    const auto importedInterfaceIdentifier = module->getIdentifierByName("Readable");
    ASSERT_TRUE(importedInterfaceIdentifier.has_value());
    const auto* importedInterface = dynamic_cast<const VnlcImportedInterface*>(importedInterfaceIdentifier.value());
    ASSERT_NE(importedInterface, nullptr);
    EXPECT_EQ(importedInterface->getGenericParameters(), std::vector<std::string>({ "T" }));

    const auto importedEnumIdentifier = module->getIdentifierByName("State");
    ASSERT_TRUE(importedEnumIdentifier.has_value());
    const auto* importedEnum = dynamic_cast<const VnlcImportedEnum*>(importedEnumIdentifier.value());
    ASSERT_NE(importedEnum, nullptr);
    const auto enumMember = importedEnum->getMemberByName("Ready");
    ASSERT_TRUE(enumMember.has_value());
    ASSERT_EQ(enumMember.value()->getAssociatedValues().size(), 1);
    EXPECT_EQ(enumMember.value()->getAssociatedValues().at("code")->getType(), "int");

    const auto typeAliasIdentifier = module->getIdentifierByName("Alias");
    ASSERT_TRUE(typeAliasIdentifier.has_value());
    const auto* typeAlias = dynamic_cast<const VnlcImportedTypeAlias*>(typeAliasIdentifier.value());
    ASSERT_NE(typeAlias, nullptr);
    EXPECT_EQ(typeAlias->getOriginalType(), "List<T>");

    const auto importedAliasIdentifier = module->getIdentifierByName("External");
    ASSERT_TRUE(importedAliasIdentifier.has_value());
    const auto* importedAlias = dynamic_cast<const VnlcImportedAlias*>(importedAliasIdentifier.value());
    ASSERT_NE(importedAlias, nullptr);
    EXPECT_EQ(importedAlias->getSource(), "dependency.module.External");
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileReaderThrowsOnEmptyFile) {
    const auto filePath = writeFile("empty.vni", "");
    const auto importItem = makeImportItem({ "module" });
    VnlcModuleInterfaceFileReader reader(filePath);

    EXPECT_THROW(static_cast<void>(reader.read()), VnlcModuleInterfaceFileReaderError);
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileReaderThrowsOnDirectory) {
    const auto directory = testDirectory / "directory.vni";
    std::filesystem::create_directories(directory);
    const auto importItem = makeImportItem({ "module" });
    VnlcModuleInterfaceFileReader reader(directory);

    EXPECT_THROW(static_cast<void>(reader.read()), VnlcModuleInterfaceFileReaderError);
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileReaderThrowsOnWrongExtension) {
    const auto filePath = writeFile("module.vnl", "{}");
    const auto importItem = makeImportItem({ "module" });
    VnlcModuleInterfaceFileReader reader(filePath);

    EXPECT_THROW(static_cast<void>(reader.read()), VnlcModuleInterfaceFileReaderError);
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileReaderThrowsOnInvalidJson) {
    const auto filePath = writeFile("module.vni", "{");
    const auto importItem = makeImportItem({ "module" });
    VnlcModuleInterfaceFileReader reader(filePath);

    EXPECT_THROW(static_cast<void>(reader.read()), VnlcModuleInterfaceFileReaderError);
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileReaderThrowsOnUnknownCategory) {
    const auto filePath = writeFile(
        "module.vni",
        R"({
    "value": {
        "category": "unknown"
    }
})"
    );
    const auto importItem = makeImportItem({ "module" });
    VnlcModuleInterfaceFileReader reader(filePath);

    EXPECT_THROW(static_cast<void>(reader.read()), VnlcModuleInterfaceFileReaderError);
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileReaderThrowsOnNonexistentFile) {
    const auto importItem = makeImportItem({ "module" });

    EXPECT_THROW(VnlcModuleInterfaceFileReader(testDirectory / "missing.vni"), std::filesystem::filesystem_error);
}

TEST_F(VnlcVniTest, VnlcPackageReaderThrowsOnMissingPackage) {
    const auto dependencyRoot = testDirectory / "dependencies";
    std::filesystem::create_directories(dependencyRoot / "available");

    VnlcConfig config{};
    config.dependencyPackageRootPaths.emplace("available", dependencyRoot / "available");

    const auto importItem = makeImportItem({ "missing", "module" });
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> packages;
    VnlcPackageReader reader(packages);

    EXPECT_THROW(reader.readPackageFromSource(importItem, config), VnlcPackageReaderError);
}

TEST_F(VnlcVniTest, VnlcPackageReaderReadsModuleInsidePackage) {
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

    VnlcConfig config{};
    config.dependencyPackageRootPaths.emplace("package", packageRoot);

    const auto importItem = makeImportItem({ "package", "api" });
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> packages;
    VnlcPackageReader reader(packages);
    reader.readPackageFromSource(importItem, config);

    const auto package = packages.find("package");
    ASSERT_NE(package, packages.end());
    const auto module = package->second->getModuleByName("api");
    ASSERT_TRUE(module.has_value());
    ASSERT_EQ(module.value()->getIdentifiers().size(), 1);
    EXPECT_EQ(dynamic_cast<const VnlcImportedLet*>(module.value()->getIdentifiers().at("value").get())->getType(), "int");
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorGeneratesLetCategory) {
    const auto config = makeGeneratorConfig(testDirectory);
    auto type = makeType("int");
    const auto* typeNode = type.get();
    auto declaration = makeValueWithMetadata(
        VnlcValueDeclarationType::Kind::LET,
        VnlcValueDeclarationType::Context::TOP_LEVEL,
        VnlcValueDeclarationType::AccessModifier::PUBLIC,
        "count",
        std::make_optional(std::move(type)),
        makeMetadata()
    );
    const auto* declarationNode = declaration.get();
    const auto semantic = makeSemanticResult({ { typeNode, VnlcPrimitiveType::intType() } });

    VnlcModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
    generator.generate();
    const auto json = readJson(generatedOutputPath(config));

    EXPECT_EQ(json["count"]["category"], "let");
    EXPECT_EQ(json["count"]["type"], "int");
    EXPECT_EQ(json["count"]["metadata"]["since"], "1.0");
    EXPECT_TRUE(json["count"]["metadata"]["draft"].is_null());
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorGeneratesNativeFuncWithParameter) {
    const auto config = makeGeneratorConfig(testDirectory);
    auto parameterType = makeType("int");
    const auto* parameterTypeNode = parameterType.get();
    auto returnType = makeType("void");
    const auto* returnTypeNode = returnType.get();

    std::vector<std::unique_ptr<VnlcValueDeclarationNode>> parameters;
    parameters.push_back(makeValue(
        VnlcValueDeclarationType::Kind::PARAMETER,
        VnlcValueDeclarationType::Context::FUNCTION,
        VnlcValueDeclarationType::AccessModifier::PUBLIC,
        "amount",
        std::make_optional(std::move(parameterType))
    ));
    auto declaration = makeFunction(
        VnlcFunctionDeclarationType::Kind::NATIVE,
        VnlcFunctionDeclarationType::Context::TOP_LEVEL,
        VnlcFunctionDeclarationType::AccessModifier::PUBLIC,
        VnlcFunctionDeclarationType::Binding::INSTANCE,
        "add",
        std::move(parameters),
        std::make_optional(std::move(returnType))
    );
    const auto* declarationNode = declaration.get();
    const auto semantic = makeSemanticResult(
        {
            { parameterTypeNode, VnlcPrimitiveType::intType() },
            { returnTypeNode, &testVoidType },
        }
    );

    VnlcModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
    generator.generate();
    const auto json = readJson(generatedOutputPath(config));

    EXPECT_EQ(json["add"]["category"], "func");
    EXPECT_EQ(json["add"]["returnType"], "void");
    EXPECT_TRUE(json["add"]["native"]);
    EXPECT_EQ(json["add"]["parameters"]["amount"]["category"], "parameter");
    EXPECT_EQ(json["add"]["parameters"]["amount"]["type"], "int");
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorGeneratesClassWithMembersAndModifiers) {
    const auto config = makeGeneratorConfig(testDirectory);
    VnlcCustomizedType baseType(VnlcCustomizedTypeKind::CLASS, "package.Base", static_cast<const VnlcTypeDeclarationNode*>(nullptr));
    VnlcCustomizedType interfaceType(VnlcCustomizedTypeKind::INTERFACE, "package.Readable", static_cast<const VnlcTypeDeclarationNode*>(nullptr));
    auto baseClass = makeType("Base");
    const auto* baseClassNode = baseClass.get();
    auto implementedInterface = makeType("Readable");
    const auto* implementedInterfaceNode = implementedInterface.get();
    auto propertyType = makeType("int");
    const auto* propertyTypeNode = propertyType.get();
    auto methodReturnType = makeType("string");
    const auto* methodReturnTypeNode = methodReturnType.get();

    std::vector<std::unique_ptr<VnlcTypeNode>> implementedInterfaces;
    implementedInterfaces.push_back(std::move(implementedInterface));

    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameters;
    genericParameters.push_back(makeIdentifier("T"));

    std::vector<std::unique_ptr<VnlcDeclarationNode>> members;
    members.push_back(makeValue(
        VnlcValueDeclarationType::Kind::STATIC_PROPERTY,
        VnlcValueDeclarationType::Context::CLASS,
        VnlcValueDeclarationType::AccessModifier::PROTECTED,
        "count",
        std::make_optional(std::move(propertyType))
    ));
    members.push_back(makeFunction(
        VnlcFunctionDeclarationType::Kind::NATIVE,
        VnlcFunctionDeclarationType::Context::CLASS,
        VnlcFunctionDeclarationType::AccessModifier::PUBLIC,
        VnlcFunctionDeclarationType::Binding::STATIC,
        "reset",
        {},
        std::nullopt
    ));
    members.push_back(makeFunction(
        VnlcFunctionDeclarationType::Kind::REGULAR,
        VnlcFunctionDeclarationType::Context::CLASS,
        VnlcFunctionDeclarationType::AccessModifier::PRIVATE,
        VnlcFunctionDeclarationType::Binding::INSTANCE,
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
            { propertyTypeNode, VnlcPrimitiveType::intType() },
            { methodReturnTypeNode, VnlcPrimitiveType::stringType() },
        }
    );

    VnlcModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
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

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorGeneratesInterfaceWithMethod) {
    const auto config = makeGeneratorConfig(testDirectory);
    auto returnType = makeType("void");
    const auto* returnTypeNode = returnType.get();

    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameters;
    genericParameters.push_back(makeIdentifier("T"));

    std::vector<std::unique_ptr<VnlcFunctionDeclarationNode>> methods;
    methods.push_back(makeFunction(
        VnlcFunctionDeclarationType::Kind::REGULAR,
        VnlcFunctionDeclarationType::Context::INTERFACE,
        VnlcFunctionDeclarationType::AccessModifier::PUBLIC,
        VnlcFunctionDeclarationType::Binding::INSTANCE,
        "run",
        {},
        std::make_optional(std::move(returnType))
    ));
    auto declaration = makeInterface("Task", std::move(genericParameters), std::move(methods));
    const auto* declarationNode = declaration.get();
    const auto semantic = makeSemanticResult(
        {
            { returnTypeNode, &testVoidType },
        }
    );

    VnlcModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
    generator.generate();
    const auto json = readJson(generatedOutputPath(config));

    EXPECT_EQ(json["Task"]["category"], "interface");
    EXPECT_EQ(json["Task"]["genericParameters"].size(), 1);
    EXPECT_EQ(json["Task"]["genericParameters"][0], "T");
    EXPECT_EQ(json["Task"]["methods"]["run"]["returnType"], "void");
    EXPECT_FALSE(json["Task"]["methods"]["run"]["native"]);
    EXPECT_FALSE(json["Task"]["methods"]["run"]["static"]);
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorGeneratesEnumWithMemberAndValue) {
    const auto config = makeGeneratorConfig(testDirectory);
    auto valueType = makeType("int");
    const auto* valueTypeNode = valueType.get();

    std::vector<std::unique_ptr<VnlcValueDeclarationNode>> associatedValues;
    associatedValues.push_back(makeValue(
        VnlcValueDeclarationType::Kind::ENUM_ASSOCIATED_VALUE,
        VnlcValueDeclarationType::Context::ENUM_MEMBER,
        VnlcValueDeclarationType::AccessModifier::PUBLIC,
        "code",
        std::make_optional(std::move(valueType))
    ));

    std::vector<std::unique_ptr<VnlcEnumMemberDeclarationNode>> members;
    members.push_back(std::make_unique<VnlcEnumMemberDeclarationNode>(makeIdentifier("Ready"), std::move(associatedValues), testToken, testToken));

    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameters;
    genericParameters.push_back(makeIdentifier("T"));

    auto declaration = makeEnum("State", std::move(genericParameters), std::move(members));
    const auto* declarationNode = declaration.get();
    const auto semantic = makeSemanticResult(
        {
            { valueTypeNode, VnlcPrimitiveType::intType() },
        }
    );

    VnlcModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
    generator.generate();
    const auto json = readJson(generatedOutputPath(config));

    EXPECT_EQ(json["State"]["category"], "enum");
    EXPECT_EQ(json["State"]["genericParameters"].size(), 1);
    EXPECT_EQ(json["State"]["genericParameters"][0], "T");
    EXPECT_EQ(json["State"]["members"]["Ready"]["category"], "enummember");
    EXPECT_EQ(json["State"]["members"]["Ready"]["associatedValues"]["code"]["category"], "enumvalue");
    EXPECT_EQ(json["State"]["members"]["Ready"]["associatedValues"]["code"]["type"], "int");
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorGeneratesTypeAliasCategory) {
    const auto config = makeGeneratorConfig(testDirectory);
    auto originalType = makeType("List<T>");
    const auto* originalTypeNode = originalType.get();

    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameters;
    genericParameters.push_back(makeIdentifier("T"));

    auto declaration = makeTypeAlias("Wrapper", std::move(genericParameters), std::move(originalType));
    const auto* declarationNode = declaration.get();
    const auto semantic = makeSemanticResult(
        {
            { originalTypeNode, VnlcPrimitiveType::intType() },
        }
    );

    VnlcModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
    generator.generate();
    const auto json = readJson(generatedOutputPath(config));

    EXPECT_EQ(json["Wrapper"]["category"], "typealias");
    EXPECT_EQ(json["Wrapper"]["genericParameters"].size(), 1);
    EXPECT_EQ(json["Wrapper"]["genericParameters"][0], "T");
    EXPECT_EQ(json["Wrapper"]["originalType"], "int");
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorGeneratesImportedAliasCategory) {
    const auto config = makeGeneratorConfig(testDirectory);
    const auto semantic = makeSemanticResult();

    VnlcModuleInterfaceFileGenerator generator({}, { "External" }, config, semantic);
    generator.generate();
    const auto json = readJson(generatedOutputPath(config));

    EXPECT_EQ(json["External"]["category"], "imported");
    EXPECT_EQ(json["External"]["source"], "External");
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorUsesInferredTypes) {
    const auto config = makeGeneratorConfig(testDirectory);
    auto declaration = makeValue(VnlcValueDeclarationType::Kind::LET, VnlcValueDeclarationType::Context::TOP_LEVEL, VnlcValueDeclarationType::AccessModifier::PUBLIC, "count", std::nullopt);
    const auto* declarationNode = declaration.get();
    const auto semantic = makeSemanticResult(
        {},
        {
            { declarationNode, VnlcPrimitiveType::intType() },
        }
    );

    VnlcModuleInterfaceFileGenerator generator({ declarationNode }, {}, config, semantic);
    generator.generate();
    const auto json = readJson(generatedOutputPath(config));

    EXPECT_EQ(json["count"]["type"], "int");
}

TEST_F(VnlcVniTest, VnlcModuleInterfaceFileGeneratorDoesNotWriteWhenOutputDirectoryIsAbsent) {
    auto config = makeGeneratorConfig(testDirectory);
    config.moduleInterfaceOutputDirectory = std::nullopt;
    const auto semantic = makeSemanticResult();

    VnlcModuleInterfaceFileGenerator generator({}, {}, config, semantic);
    generator.generate();

    EXPECT_FALSE(std::filesystem::exists(testDirectory / "interfaces"));
}
