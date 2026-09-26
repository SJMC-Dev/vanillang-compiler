#include "parser/Parser.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/expression/NoneExpressionNode.hpp"
#include "ast/expression/PrimitiveTypeExpressionKind.hpp"
#include "ast/expression/PrimitiveTypeExpressionNode.hpp"
#include "ast/expression/RangeExpressionNode.hpp"
#include "ast/statement/ForStatementNode.hpp"
#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "ast/typeref/PrimitiveTypeReferenceKind.hpp"
#include "ast/typeref/PrimitiveTypeReferenceNode.hpp"
#include "collector/Collector.hpp"
#include "config/Config.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

namespace vnlc {

    namespace {

        CollectionResult collectModule(std::istream& input, const Config& config) {
            Lexer lexer(input);
            Collector collector(std::move(lexer));
            return collector.collect(config);
        }

        ParseResult parseModule(std::istream& input, const Config& config) {
            auto collectionResult = collectModule(input, config);

            input.clear();
            input.seekg(0);

            Lexer lexer(input);
            Parser parser(std::move(lexer), collectionResult);
            return parser.parse(config);
        }

    } // namespace

    TEST(ParserTest, SimpleModule) {
        const auto inputDir = std::filesystem::path(VNLC_TEST_SOURCE_DIR) / "inputs";

        std::ifstream input(inputDir / "main.vnl");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = std::filesystem::canonical(inputDir),
            .inputFilePath = std::filesystem::canonical(inputDir / "main.vnl"),
            .outputDirectory = std::nullopt,
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
    }

    TEST(ParserTest, DistinguishesLoopVariablesFromOrdinaryVariables) {
        std::stringstream input("func test() {\n    let value = 1\n    for (let item in 1..3) {}\n}\n");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = std::filesystem::current_path(),
            .inputFilePath = std::filesystem::current_path() / "test.vnl",
            .outputDirectory = std::nullopt,
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(function, nullptr);
        ASSERT_TRUE(function->getBody().has_value());

        const auto& statements = function->getBody().value()->getStatements();
        ASSERT_EQ(statements.size(), 2);

        const auto* variableStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(variableStatement, nullptr);
        EXPECT_EQ(variableStatement->getVariableDeclaration().getKind(), ValueDeclarationKind::Kind::LET);
        EXPECT_TRUE(variableStatement->getVariableDeclaration().getInitializer().has_value());

        const auto* forStatement = dynamic_cast<const ForStatementNode*>(statements[1].get());
        ASSERT_NE(forStatement, nullptr);
        EXPECT_EQ(forStatement->getLoopVariable().getKind(), ValueDeclarationKind::Kind::LOOP_VARIABLE);
        EXPECT_FALSE(forStatement->getLoopVariable().getInitializer().has_value());
    }

    TEST(ParserTest, PropertyDeclarationsUseValueDeclarationNode) {
        std::stringstream input("class Example {\n    private value: string\n    static count: int = 0\n}\n");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = std::filesystem::current_path(),
            .inputFilePath = std::filesystem::current_path() / "test.vnl",
            .outputDirectory = std::nullopt,
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(classDeclaration, nullptr);
        ASSERT_EQ(classDeclaration->getMemberDeclarations().size(), 2);

        const auto* instanceProperty = dynamic_cast<const ValueDeclarationNode*>(classDeclaration->getMemberDeclarations()[0].get());
        ASSERT_NE(instanceProperty, nullptr);
        EXPECT_EQ(instanceProperty->getKind(), ValueDeclarationKind::Kind::INSTANCE_PROPERTY);
        EXPECT_EQ(instanceProperty->getContext(), ValueDeclarationKind::Context::CLASS);
        EXPECT_EQ(instanceProperty->getAccessModifier(), ValueDeclarationKind::AccessModifier::PRIVATE);
        EXPECT_TRUE(instanceProperty->getType().has_value());
        EXPECT_FALSE(instanceProperty->getInitializer().has_value());

        const auto* staticProperty = dynamic_cast<const ValueDeclarationNode*>(classDeclaration->getMemberDeclarations()[1].get());
        ASSERT_NE(staticProperty, nullptr);
        EXPECT_EQ(staticProperty->getKind(), ValueDeclarationKind::Kind::STATIC_PROPERTY);
        EXPECT_EQ(staticProperty->getContext(), ValueDeclarationKind::Context::CLASS);
        EXPECT_EQ(staticProperty->getAccessModifier(), ValueDeclarationKind::AccessModifier::PUBLIC);
        EXPECT_TRUE(staticProperty->getType().has_value());
        EXPECT_TRUE(staticProperty->getInitializer().has_value());
    }

    TEST(ParserTest, ParsesNoneAsPrimaryExpression) {
        std::stringstream input("func test() {\n    let value = none\n    let range = 1..none\n}\n");

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = std::filesystem::current_path(),
            .inputFilePath = std::filesystem::current_path() / "test.vnl",
            .outputDirectory = std::nullopt,
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(function, nullptr);
        ASSERT_TRUE(function->getBody().has_value());

        const auto& statements = function->getBody().value()->getStatements();
        ASSERT_EQ(statements.size(), 2);

        const auto* valueStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(valueStatement, nullptr);
        ASSERT_TRUE(valueStatement->getVariableDeclaration().getInitializer().has_value());
        EXPECT_NE(dynamic_cast<const NoneExpressionNode*>(valueStatement->getVariableDeclaration().getInitializer().value().get()), nullptr);

        const auto* rangeStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(rangeStatement, nullptr);
        ASSERT_TRUE(rangeStatement->getVariableDeclaration().getInitializer().has_value());

        const auto* rangeExpression = dynamic_cast<const RangeExpressionNode*>(rangeStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(rangeExpression, nullptr);
        ASSERT_TRUE(rangeExpression->getEnd().has_value());
        EXPECT_NE(dynamic_cast<const NoneExpressionNode*>(rangeExpression->getEnd().value().get()), nullptr);
    }

    TEST(ParserTest, DistinguishesPrimitiveAndCustomizedTypeReferences) {
        std::stringstream input(
            "class Example {}\n"
            "func test() {\n"
            "    let primitiveType = int\n"
            "    let identifier = value\n"
            "    let primitiveValue: int? = none\n"
            "    let customizedValue: Example = none\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = std::filesystem::current_path(),
            .inputFilePath = std::filesystem::current_path() / "test.vnl",
            .outputDirectory = std::nullopt,
            .dependencyPackageRootPaths = {},
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 2);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations()[1].get());
        ASSERT_NE(function, nullptr);
        ASSERT_TRUE(function->getBody().has_value());

        const auto& statements = function->getBody().value()->getStatements();
        ASSERT_EQ(statements.size(), 4);

        const auto* primitiveExpressionStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(primitiveExpressionStatement, nullptr);
        ASSERT_TRUE(primitiveExpressionStatement->getVariableDeclaration().getInitializer().has_value());
        const auto* primitiveExpression = dynamic_cast<const PrimitiveTypeExpressionNode*>(primitiveExpressionStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(primitiveExpression, nullptr);
        EXPECT_EQ(primitiveExpression->getKind(), PrimitiveTypeExpressionKind::INT);

        const auto* identifierStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(identifierStatement, nullptr);
        ASSERT_TRUE(identifierStatement->getVariableDeclaration().getInitializer().has_value());
        EXPECT_NE(dynamic_cast<const IdentifierLikeExpressionNode*>(identifierStatement->getVariableDeclaration().getInitializer().value().get()), nullptr);

        const auto* primitiveValueStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[2].get());
        ASSERT_NE(primitiveValueStatement, nullptr);
        ASSERT_TRUE(primitiveValueStatement->getVariableDeclaration().getType().has_value());
        const auto* primitiveReference = dynamic_cast<const PrimitiveTypeReferenceNode*>(primitiveValueStatement->getVariableDeclaration().getType().value().get());
        ASSERT_NE(primitiveReference, nullptr);
        EXPECT_EQ(primitiveReference->getKind(), PrimitiveTypeReferenceKind::INT);
        EXPECT_TRUE(primitiveReference->hasQuestionMarkSuffix());

        const auto* customizedValueStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[3].get());
        ASSERT_NE(customizedValueStatement, nullptr);
        ASSERT_TRUE(customizedValueStatement->getVariableDeclaration().getType().has_value());
        const auto* customizedReference = dynamic_cast<const CustomizedTypeReferenceNode*>(customizedValueStatement->getVariableDeclaration().getType().value().get());
        ASSERT_NE(customizedReference, nullptr);
        ASSERT_EQ(customizedReference->getNameParts().size(), 1);
        EXPECT_EQ(customizedReference->getNameParts().front()->getIdentifierString(), "Example");
        EXPECT_FALSE(customizedReference->hasQuestionMarkSuffix());
        EXPECT_TRUE(customizedReference->getGenericArguments().empty());
    }

} // namespace vnlc
