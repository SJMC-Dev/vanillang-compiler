#include "parser/Parser.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/expression/BinaryExpressionNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionNode.hpp"
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

    TEST(ParserTest, ParsesGenericArgumentsForLocalTypeDeclarationsOnly) {
        std::stringstream input(
            "class Box {}\n"
            "class Pair {}\n"
            "class Item {}\n"
            "func test() {\n"
            "    let generic = Box<Pair<Item>>\n"
            "    let member = Box<Item.value>\n"
            "    let compared = left < right\n"
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
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 4);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations()[3].get());
        ASSERT_NE(function, nullptr);
        ASSERT_TRUE(function->getBody().has_value());

        const auto& statements = function->getBody().value()->getStatements();
        ASSERT_EQ(statements.size(), 3);

        const auto* genericStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(genericStatement, nullptr);
        ASSERT_TRUE(genericStatement->getVariableDeclaration().getInitializer().has_value());
        const auto* genericExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(genericStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(genericExpression, nullptr);
        EXPECT_EQ(genericExpression->getName().getIdentifierString(), "Box");
        ASSERT_EQ(genericExpression->getGenericArguments().size(), 1);
        const auto* pairExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(genericExpression->getGenericArguments().front().get());
        ASSERT_NE(pairExpression, nullptr);
        EXPECT_EQ(pairExpression->getName().getIdentifierString(), "Pair");
        ASSERT_EQ(pairExpression->getGenericArguments().size(), 1);
        const auto* itemExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(pairExpression->getGenericArguments().front().get());
        ASSERT_NE(itemExpression, nullptr);
        EXPECT_EQ(itemExpression->getName().getIdentifierString(), "Item");
        EXPECT_TRUE(itemExpression->getGenericArguments().empty());

        const auto* memberStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(memberStatement, nullptr);
        ASSERT_TRUE(memberStatement->getVariableDeclaration().getInitializer().has_value());
        const auto* memberExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(memberStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(memberExpression, nullptr);
        ASSERT_EQ(memberExpression->getGenericArguments().size(), 1);
        const auto* memberArgument = dynamic_cast<const MemberAccessExpressionNode*>(memberExpression->getGenericArguments().front().get());
        ASSERT_NE(memberArgument, nullptr);
        EXPECT_NE(dynamic_cast<const IdentifierLikeExpressionNode*>(&memberArgument->getObject()), nullptr);
        EXPECT_EQ(memberArgument->getMember().getName().getIdentifierString(), "value");

        const auto* comparedStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[2].get());
        ASSERT_NE(comparedStatement, nullptr);
        ASSERT_TRUE(comparedStatement->getVariableDeclaration().getInitializer().has_value());
        const auto* comparedExpression = dynamic_cast<const BinaryExpressionNode*>(comparedStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(comparedExpression, nullptr);
        EXPECT_EQ(comparedExpression->getKind(), BinaryExpressionKind::LESS_THAN);
        EXPECT_NE(dynamic_cast<const IdentifierLikeExpressionNode*>(&comparedExpression->getLeft()), nullptr);
        EXPECT_NE(dynamic_cast<const IdentifierLikeExpressionNode*>(&comparedExpression->getRight()), nullptr);
    }

    TEST(ParserTest, ParsesGenericArgumentsForImportedTypeDeclarations) {
        const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
        auto testDirectory = std::filesystem::temp_directory_path() / ("vnlctest-parser-" + std::string(testInfo->name()));
        std::filesystem::remove_all(testDirectory);
        std::filesystem::create_directories(testDirectory / "first_source");
        std::filesystem::create_directories(testDirectory / "second_source");

        std::ofstream firstModuleInterface(testDirectory / "first_source" / "models.vni");
        firstModuleInterface << R"({"Box":{"category":"class","genericParameters":["T"],"properties":{},"methods":{},"baseClass":null,"implementedInterfaces":[],"final":false}})";
        firstModuleInterface.close();

        std::ofstream secondModuleInterface(testDirectory / "second_source" / "models.vni");
        secondModuleInterface << R"({"Box":{"category":"interface","genericParameters":["T"],"methods":{}}})";
        secondModuleInterface.close();

        std::stringstream input(
            "import first.models as firstModels\n"
            "import second.models as secondModels\n"
            "import first.models.Box as DirectBox\n"
            "func test() {\n"
            "    let first = firstModels.Box<Item>\n"
            "    let second = secondModels.Box<Value>\n"
            "    let direct = DirectBox<Member>\n"
            "}\n"
        );

        Config config{
            .mode = RunningMode::COMPILE,
            .vanillangVersion = "1.0",
            .minecraftVersion = "26.1.2",
            .packageRootPath = testDirectory / "source_package",
            .inputFilePath = testDirectory / "source_package" / "test.vnl",
            .outputDirectory = std::nullopt,
            .dependencyPackageRootPaths = {
                { "first", testDirectory / "first_source" },
                { "second", testDirectory / "second_source" },
            },
            .optimizationLevel = std::nullopt,
        };

        auto result = parseModule(input, config);
        const auto& module = result.getModuleNode();
        ASSERT_EQ(module.getTopIdentifierDeclarations().size(), 1);

        const auto* function = dynamic_cast<const FunctionDeclarationNode*>(module.getTopIdentifierDeclarations().front().get());
        ASSERT_NE(function, nullptr);
        ASSERT_TRUE(function->getBody().has_value());

        const auto& statements = function->getBody().value()->getStatements();
        ASSERT_EQ(statements.size(), 3);

        const auto firstStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[0].get());
        ASSERT_NE(firstStatement, nullptr);
        ASSERT_TRUE(firstStatement->getVariableDeclaration().getInitializer().has_value());
        const auto* firstExpression = dynamic_cast<const MemberAccessExpressionNode*>(firstStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(firstExpression, nullptr);
        EXPECT_EQ(firstExpression->getMember().getName().getIdentifierString(), "Box");
        ASSERT_EQ(firstExpression->getMember().getGenericArguments().size(), 1);
        const auto* firstArgument = dynamic_cast<const IdentifierLikeExpressionNode*>(firstExpression->getMember().getGenericArguments().front().get());
        ASSERT_NE(firstArgument, nullptr);
        EXPECT_EQ(firstArgument->getName().getIdentifierString(), "Item");

        const auto secondStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[1].get());
        ASSERT_NE(secondStatement, nullptr);
        ASSERT_TRUE(secondStatement->getVariableDeclaration().getInitializer().has_value());
        const auto* secondExpression = dynamic_cast<const MemberAccessExpressionNode*>(secondStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(secondExpression, nullptr);
        EXPECT_EQ(secondExpression->getMember().getName().getIdentifierString(), "Box");
        ASSERT_EQ(secondExpression->getMember().getGenericArguments().size(), 1);
        const auto* secondArgument = dynamic_cast<const IdentifierLikeExpressionNode*>(secondExpression->getMember().getGenericArguments().front().get());
        ASSERT_NE(secondArgument, nullptr);
        EXPECT_EQ(secondArgument->getName().getIdentifierString(), "Value");

        const auto directStatement = dynamic_cast<const VariableDeclarationStatementNode*>(statements[2].get());
        ASSERT_NE(directStatement, nullptr);
        ASSERT_TRUE(directStatement->getVariableDeclaration().getInitializer().has_value());
        const auto* directExpression = dynamic_cast<const IdentifierLikeExpressionNode*>(directStatement->getVariableDeclaration().getInitializer().value().get());
        ASSERT_NE(directExpression, nullptr);
        EXPECT_EQ(directExpression->getName().getIdentifierString(), "DirectBox");
        ASSERT_EQ(directExpression->getGenericArguments().size(), 1);
        const auto* directArgument = dynamic_cast<const IdentifierLikeExpressionNode*>(directExpression->getGenericArguments().front().get());
        ASSERT_NE(directArgument, nullptr);
        EXPECT_EQ(directArgument->getName().getIdentifierString(), "Member");

        std::error_code error;
        std::filesystem::remove_all(testDirectory, error);
    }

} // namespace vnlc
