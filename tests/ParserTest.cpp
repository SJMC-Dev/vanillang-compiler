#include "parser/Parser.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/expression/NoneExpressionNode.hpp"
#include "ast/expression/RangeExpressionNode.hpp"
#include "ast/statement/ForStatementNode.hpp"
#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include "config/Config.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

namespace vnlc {

    TEST(ParserTest, SimpleModule) {
        const auto inputDir = std::filesystem::path(VNLC_TEST_SOURCE_DIR) / "inputs";

        std::ifstream input(inputDir / "main.vnl");
        Lexer lexer(input);
        Parser parser(std::move(lexer));

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

        auto result = parser.parse(std::move(config));
    }

    TEST(ParserTest, DistinguishesLoopVariablesFromOrdinaryVariables) {
        std::stringstream input("func test() {\n    let value = 1\n    for (let item in 1..3) {}\n}\n");
        Lexer lexer(input);
        Parser parser(std::move(lexer));

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

        auto module = parser.parse(config);
        ASSERT_EQ(module->getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module->getTopIdentifierDeclarations().front().get());
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
        Lexer lexer(input);
        Parser parser(std::move(lexer));

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

        auto module = parser.parse(config);
        ASSERT_EQ(module->getTopIdentifierDeclarations().size(), 1);

        const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(module->getTopIdentifierDeclarations().front().get());
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
        Lexer lexer(input);
        Parser parser(std::move(lexer));

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

        auto module = parser.parse(config);
        ASSERT_EQ(module->getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module->getTopIdentifierDeclarations().front().get());
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

} // namespace vnlc
