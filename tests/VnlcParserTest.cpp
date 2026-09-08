#include "parser/VnlcParser.hpp"
#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include "ast/statement/VnlcForStatementNode.hpp"
#include "ast/statement/VnlcVariableDeclarationStatementNode.hpp"
#include "config/VnlcConfig.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

TEST(VnlcParserTest, SimpleModule) {
    const auto inputDir = std::filesystem::path(VNLC_TEST_SOURCE_DIR) / "inputs";

    std::ifstream input(inputDir / "main.vnl");
    VnlcLexer lexer(input);
    VnlcParser parser(std::move(lexer));

    VnlcConfig config{
        .mode = VnlcRunningMode::COMPILE,
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

TEST(VnlcParserTest, DistinguishesLoopVariablesFromOrdinaryVariables) {
    std::stringstream input("func test() {\n    let value = 1\n    for (let item in 1..3) {}\n}\n");
    VnlcLexer lexer(input);
    VnlcParser parser(std::move(lexer));

    VnlcConfig config{
        .mode = VnlcRunningMode::COMPILE,
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

    const auto* function = dynamic_cast<const VnlcFunctionDeclarationNode*>(module->getTopIdentifierDeclarations().front().get());
    ASSERT_NE(function, nullptr);
    ASSERT_TRUE(function->getBody().has_value());

    const auto& statements = function->getBody().value()->getStatements();
    ASSERT_EQ(statements.size(), 2);

    const auto* variableStatement = dynamic_cast<const VnlcVariableDeclarationStatementNode*>(statements[0].get());
    ASSERT_NE(variableStatement, nullptr);
    EXPECT_EQ(variableStatement->getVariableDeclaration().getKind(), VnlcValueDeclarationType::Kind::LET);
    EXPECT_TRUE(variableStatement->getVariableDeclaration().getInitializer().has_value());

    const auto* forStatement = dynamic_cast<const VnlcForStatementNode*>(statements[1].get());
    ASSERT_NE(forStatement, nullptr);
    EXPECT_EQ(forStatement->getLoopVariable().getKind(), VnlcValueDeclarationType::Kind::LOOP_VARIABLE);
    EXPECT_FALSE(forStatement->getLoopVariable().getInitializer().has_value());
}
