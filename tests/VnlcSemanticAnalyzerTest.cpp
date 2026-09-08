#include "semantic/VnlcSemanticAnalyzer.hpp"
#include "ast/declaration/VnlcClassDeclarationNode.hpp"
#include "ast/declaration/VnlcEnumDeclarationNode.hpp"
#include "ast/declaration/VnlcInterfaceDeclarationNode.hpp"
#include "ast/declaration/VnlcTypeAliasDeclarationNode.hpp"
#include "config/VnlcConfig.hpp"
#include "lexer/VnlcLexer.hpp"
#include "parser/VnlcParser.hpp"
#include "semantic/VnlcSemanticAnalysisResult.hpp"
#include "type/VnlcCustomizedTypeKind.hpp"
#include "type/VnlcCustomizedTypeOrigin.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

namespace {

    VnlcConfig makeConfig(std::string_view moduleFileName) {
        const auto packageRootPath = std::filesystem::current_path() / "semantic_test_package";

        return VnlcConfig{
            .mode = VnlcRunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = packageRootPath,
            .inputFilePath = packageRootPath / "models" / moduleFileName,
            .outputDirectory = std::nullopt,
            .moduleInterfaceOutputDirectory = std::nullopt,
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };
    }

    std::unique_ptr<VnlcModuleNode> parseModule(std::string_view source, const VnlcConfig& config) {
        std::stringstream input{ std::string(source) };
        VnlcLexer lexer(input);
        VnlcParser parser(std::move(lexer));
        return parser.parse(config);
    }

    void expectRegisteredCustomizedType(
        const VnlcSemanticAnalysisResult& result,
        std::string_view fullTypeName,
        VnlcCustomizedTypeKind expectedKind,
        const VnlcTypeDeclarationNode* expectedDeclaration
    ) {
        SCOPED_TRACE(fullTypeName);

        const auto customizedType = result.getCustomizedTypeByFullTypeName(fullTypeName);
        ASSERT_TRUE(customizedType.has_value());
        ASSERT_NE(customizedType.value(), nullptr);
        EXPECT_EQ(customizedType.value()->getFullTypeName(), fullTypeName);
        EXPECT_EQ(customizedType.value()->getCustomizedKind(), expectedKind);
        EXPECT_EQ(customizedType.value()->getOrigin(), VnlcCustomizedTypeOrigin::LOCAL);
        EXPECT_EQ(customizedType.value()->getLocalDeclaration(), expectedDeclaration);
        EXPECT_EQ(customizedType.value()->getImportedDeclaration(), nullptr);
    }

} // namespace

TEST(VnlcSemanticAnalyzerTest, RegistersLocalCustomizedTypes) {
    constexpr std::string_view source = R"(
class SampleClass {}
interface SampleInterface {}
enum SampleEnum {}
type SampleAlias = int
)";
    const auto config = makeConfig("types.vnl");
    auto module = parseModule(source, config);

    VnlcSemanticAnalyzer analyzer(*module);
    const auto result = analyzer.analyze(config);

    ASSERT_FALSE(result.hasErrors());
    const auto& declarations = module->getTopIdentifierDeclarations();
    ASSERT_EQ(declarations.size(), 4);

    const auto* classDeclaration = dynamic_cast<const VnlcClassDeclarationNode*>(declarations[0].get());
    const auto* interfaceDeclaration = dynamic_cast<const VnlcInterfaceDeclarationNode*>(declarations[1].get());
    const auto* enumDeclaration = dynamic_cast<const VnlcEnumDeclarationNode*>(declarations[2].get());
    const auto* typeAliasDeclaration = dynamic_cast<const VnlcTypeAliasDeclarationNode*>(declarations[3].get());
    ASSERT_NE(classDeclaration, nullptr);
    ASSERT_NE(interfaceDeclaration, nullptr);
    ASSERT_NE(enumDeclaration, nullptr);
    ASSERT_NE(typeAliasDeclaration, nullptr);

    expectRegisteredCustomizedType(result, "semantic_test_package.models.types.SampleClass", VnlcCustomizedTypeKind::CLASS, classDeclaration);
    expectRegisteredCustomizedType(result, "semantic_test_package.models.types.SampleInterface", VnlcCustomizedTypeKind::INTERFACE, interfaceDeclaration);
    expectRegisteredCustomizedType(result, "semantic_test_package.models.types.SampleEnum", VnlcCustomizedTypeKind::ENUM, enumDeclaration);
    expectRegisteredCustomizedType(result, "semantic_test_package.models.types.SampleAlias", VnlcCustomizedTypeKind::TYPE_ALIAS, typeAliasDeclaration);
}

TEST(VnlcSemanticAnalyzerTest, DoesNotRegisterInvalidDeclarationAndContinuesRegisteringValidDeclarations) {
    constexpr std::string_view source = R"(
class InvalidClass {
    func duplicated() {}
    func duplicated() {}
}
interface ValidInterface {}
)";
    const auto config = makeConfig("registration_errors.vnl");
    auto module = parseModule(source, config);

    VnlcSemanticAnalyzer analyzer(*module);
    const auto result = analyzer.analyze(config);

    ASSERT_TRUE(result.hasErrors());
    ASSERT_EQ(result.getErrors().size(), 1);
    EXPECT_EQ(result.getErrors().front().getMessage(), "Redeclaration of class member 'duplicated'");
    EXPECT_FALSE(result.getCustomizedTypeByFullTypeName("semantic_test_package.models.registration_errors.InvalidClass").has_value());

    const auto& declarations = module->getTopIdentifierDeclarations();
    ASSERT_EQ(declarations.size(), 2);
    const auto* validInterfaceDeclaration = dynamic_cast<const VnlcInterfaceDeclarationNode*>(declarations[1].get());
    ASSERT_NE(validInterfaceDeclaration, nullptr);
    expectRegisteredCustomizedType(result, "semantic_test_package.models.registration_errors.ValidInterface", VnlcCustomizedTypeKind::INTERFACE, validInterfaceDeclaration);
}
