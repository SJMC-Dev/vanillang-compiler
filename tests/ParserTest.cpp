#include "parser/Parser.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
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
        EXPECT_EQ(variableStatement->getVariableDeclaration().getKind(), ValueDeclarationType::Kind::LET);
        EXPECT_TRUE(variableStatement->getVariableDeclaration().getInitializer().has_value());

        const auto* forStatement = dynamic_cast<const ForStatementNode*>(statements[1].get());
        ASSERT_NE(forStatement, nullptr);
        EXPECT_EQ(forStatement->getLoopVariable().getKind(), ValueDeclarationType::Kind::LOOP_VARIABLE);
        EXPECT_FALSE(forStatement->getLoopVariable().getInitializer().has_value());
    }

} // namespace vnlc
