#include "semantic/SemanticAnalyzer.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/statement/ExpressionStatementNode.hpp"
#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "collector/CollectionResult.hpp"
#include "collector/Collector.hpp"
#include "config/Config.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticContext.hpp"
#include "semantic/SemanticResult.hpp"
#include "type/CustomizedTypeKind.hpp"
#include "type/PrimitiveType.hpp"
#include "type/TypeExpressionType.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace vnlc {

    namespace {

        Config makeConfig(std::string_view moduleFileName) {
            const auto packageRootPath = std::filesystem::current_path() / "semantic_test_package";

            return Config{
                .mode = RunningMode::COMPILE,
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

        std::shared_ptr<const ModuleNode> parseModule(std::string_view source, const Config& config) {
            std::stringstream input{ std::string(source) };
            Lexer collectorLexer(input);
            Collector collector(std::move(collectorLexer));
            auto collectionResult = collector.collect(config);

            input.clear();
            input.seekg(0);

            Lexer lexer(input);
            Parser parser(std::move(lexer), collectionResult);
            auto parseResult = std::make_shared<ParseResult>(parser.parse(config));
            return std::shared_ptr<const ModuleNode>(parseResult, &parseResult->getModuleNode());
        }

        CollectionResult collectModule(std::string_view source, const Config& config) {
            std::stringstream input{ std::string(source) };
            Lexer lexer(input);
            Collector collector(std::move(lexer));
            return collector.collect(config);
        }

    } // namespace

    class SemanticAnalyzerAccessTest : public testing::Test {
    protected:
        const Config config = makeConfig("access.vnl");
        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        std::shared_ptr<const ModuleNode> module;
        std::unique_ptr<ClassDeclarationNode> baseClass;
        std::unique_ptr<ClassDeclarationNode> privateShadow;
        std::vector<std::shared_ptr<const ModuleNode>> accessModules;
        std::vector<std::unique_ptr<TypeExpressionType>> typeExpressions;
        std::unique_ptr<SemanticAnalyzer> analyzer;

        void SetUp() override {
            module = parseModule(
                R"(
class Derived extends Base {}
class Descendant extends Derived {}
class Unrelated {}
class MissingBase extends Unknown {}
class CycleA extends CycleB {}
class CycleB extends CycleA {}
class PublicShadow extends Base {
    func protectedMember() {}
    func privateMember() {}
}
class GenericShadow<protectedMember> extends Base {}
class GenericPrivateShadow<privateMember> extends Base {}
)",
                config
            );
            const Token token(TokenKind::IDENTIFIER, "Base", 1, 1, 0);
            std::vector<std::unique_ptr<DeclarationNode>> members;
            const auto addProperty = [&](std::string_view name, ValueDeclarationKind::AccessModifier accessModifier) {
                members.push_back(
                    std::make_unique<ValueDeclarationNode>(
                        ValueDeclarationKind::Kind::INSTANCE_PROPERTY,
                        ValueDeclarationKind::Context::CLASS,
                        accessModifier,
                        std::make_unique<IdentifierNode>(name, token, token),
                        std::nullopt,
                        std::nullopt,
                        token,
                        token
                    )
                );
            };
            addProperty("protectedMember", ValueDeclarationKind::AccessModifier::PROTECTED);
            addProperty("privateMember", ValueDeclarationKind::AccessModifier::PRIVATE);
            addProperty("publicMember", ValueDeclarationKind::AccessModifier::PUBLIC);
            members.push_back(
                std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationKind::Kind::NATIVE,
                    FunctionDeclarationKind::Context::CLASS,
                    FunctionDeclarationKind::AccessModifier::PROTECTED,
                    FunctionDeclarationKind::Binding::STATIC,
                    std::make_unique<IdentifierNode>("protectedMethod", token, token),
                    std::vector<std::unique_ptr<ValueDeclarationNode>>{},
                    std::nullopt,
                    std::nullopt,
                    token,
                    token
                )
            );
            members.push_back(
                std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationKind::Kind::NATIVE,
                    FunctionDeclarationKind::Context::CLASS,
                    FunctionDeclarationKind::AccessModifier::PROTECTED,
                    FunctionDeclarationKind::Binding::INSTANCE,
                    std::make_unique<IdentifierNode>("protectedInstanceMethod", token, token),
                    std::vector<std::unique_ptr<ValueDeclarationNode>>{},
                    std::nullopt,
                    std::nullopt,
                    token,
                    token
                )
            );
            baseClass = std::make_unique<ClassDeclarationNode>(
                false,
                std::make_unique<IdentifierNode>("Base", token, token),
                std::nullopt,
                std::vector<std::unique_ptr<TypeReferenceNode>>{},
                std::vector<std::unique_ptr<IdentifierNode>>{},
                std::move(members),
                token,
                token
            );
            members.clear();
            addProperty("protectedMember", ValueDeclarationKind::AccessModifier::PRIVATE);
            std::vector<std::unique_ptr<IdentifierNode>> baseName;
            baseName.push_back(std::make_unique<IdentifierNode>("Base", token, token));
            privateShadow = std::make_unique<ClassDeclarationNode>(
                false,
                std::make_unique<IdentifierNode>("PrivateShadow", token, token),
                std::make_unique<CustomizedTypeReferenceNode>(false, std::move(baseName), std::vector<std::unique_ptr<TypeReferenceNode>>{}, token, token),
                std::vector<std::unique_ptr<TypeReferenceNode>>{},
                std::vector<std::unique_ptr<IdentifierNode>>{},
                std::move(members),
                token,
                token
            );
            analyzer = std::make_unique<SemanticAnalyzer>(*module, imports);
            auto& context = analyzer->context;
            context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, module.get()));
            std::vector<const ClassDeclarationNode*> classes{ baseClass.get(), privateShadow.get() };
            for (const auto& declaration : module->getTopIdentifierDeclarations()) {
                classes.push_back(&dynamic_cast<const ClassDeclarationNode&>(*declaration));
            }
            for (const auto* classDeclaration : classes) {
                ASSERT_TRUE(context.currentScope().declare(Symbol(SymbolKind::CLASS, SymbolAccessModifier::PUBLIC, classDeclaration->getName().getIdentifierString(), classDeclaration)));
                const auto fullName = std::string(module->getFullName()) + "." + std::string(classDeclaration->getName().getIdentifierString());
                context.registerCustomizedType(std::make_unique<CustomizedType>(CustomizedTypeKind::CLASS, fullName, std::vector<const Type*>{}, classDeclaration));
            }
            accessModules.push_back(parseModule("let privateMember = 0\n", config));
            ASSERT_TRUE(
                context.currentScope().declare(Symbol(SymbolKind::VARIABLE, SymbolAccessModifier::PUBLIC, "privateMember", accessModules.back()->getTopIdentifierDeclarations().front().get()))
            );
            for (const auto* classDeclaration : classes) {
                analyzer->checkClassDeclaration(*classDeclaration, config);
            }
            for (const auto* classDeclaration : classes) {
                if (!classDeclaration->getBaseClass().has_value()) continue;
                const auto* baseType = dynamic_cast<const CustomizedTypeReferenceNode*>(classDeclaration->getBaseClass().value().get());
                ASSERT_NE(baseType, nullptr);
                if (const auto* resolvedType = findType(baseType->getNameParts().front()->getIdentifierString())) {
                    context.mapType(baseType, resolvedType);
                }
            }
            context.popScope();
            ASSERT_TRUE(context.getErrors().empty()) << context.getErrors().front().getMessage();
        }

        const CustomizedType* findType(std::string_view name) const {
            return analyzer->context.getCustomizedTypeByFullTypeName(std::string(module->getFullName()) + "." + std::string(name));
        }

        const ClassDeclarationNode* findClass(std::string_view name) const {
            if (name == "Base") return baseClass.get();
            if (name == "PrivateShadow") return privateShadow.get();
            for (const auto& declaration : module->getTopIdentifierDeclarations()) {
                const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(declaration.get());
                if (classDeclaration->getName().getIdentifierString() == name) return classDeclaration;
            }
            return nullptr;
        }

        void leaveBaseScopeUnbuilt() {
            auto& context = analyzer->context;
            context = SemanticContext{};
            const auto fullName = std::string(module->getFullName()) + ".Base";
            context.registerCustomizedType(std::make_unique<CustomizedType>(CustomizedTypeKind::CLASS, fullName, std::vector<const Type*>{}, baseClass.get()));
            context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, module.get()));
            context.popScope();
        }

        void prepareDerivedScopeWithoutBaseScope() {
            leaveBaseScopeUnbuilt();
            auto& context = analyzer->context;
            const auto& derivedClass = dynamic_cast<const ClassDeclarationNode&>(*module->getTopIdentifierDeclarations().front());
            const auto fullName = std::string(module->getFullName()) + ".Derived";
            context.registerCustomizedType(std::make_unique<CustomizedType>(CustomizedTypeKind::CLASS, fullName, std::vector<const Type*>{}, &derivedClass));
            context.mapType(derivedClass.getBaseClass().value().get(), findType("Base"));
            context.pushScope(std::make_unique<Scope>(ScopeKind::CLASS, context.getScopeByAstNode(module.get()), &derivedClass));
            context.popScope();
        }

        void prepareUnregisteredDerivedScope() {
            leaveBaseScopeUnbuilt();
            auto& context = analyzer->context;
            const auto* derivedClass = findClass("Derived");
            context.mapType(derivedClass->getBaseClass().value().get(), findType("Base"));
            context.pushScope(std::make_unique<Scope>(ScopeKind::CLASS, context.getScopeByAstNode(module.get()), derivedClass));
        }

        bool canAccess(std::string_view receiver, std::string_view accessor, std::string_view member, bool typeExpression = false, std::string_view object = "object") {
            accessModules.push_back(parseModule("func inspect() {\n" + std::string(object) + "." + std::string(member) + "\n}\n", config));
            const auto& function = dynamic_cast<const FunctionDeclarationNode&>(*accessModules.back()->getTopIdentifierDeclarations().front());
            const auto& statement = dynamic_cast<const ExpressionStatementNode&>(*function.getBody().value()->getStatements().front());
            const auto& access = dynamic_cast<const MemberAccessExpressionNode&>(statement.getExpression());
            auto& context = analyzer->context;
            const auto* parent = context.getScopeByAstNode(accessor.empty() ? static_cast<const AstNode*>(module.get()) : findType(accessor)->getLocalNode());
            context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, parent, &function));
            const Type* receiverType = findType(receiver);
            if (typeExpression) {
                typeExpressions.push_back(std::make_unique<TypeExpressionType>(receiverType));
                receiverType = typeExpressions.back().get();
            }
            context.mapInferredExpressionType(&access.getObject(), receiverType);
            const bool allowed = analyzer->checkAccessModifier(access);
            context.popScope();
            return allowed;
        }

        bool canAccessIdentifier(std::string_view accessor, std::string_view identifier, std::optional<SymbolKind> localSymbolKind = std::nullopt, bool nestedBlock = false) {
            const auto parameter = localSymbolKind == SymbolKind::PARAMETER ? std::string(identifier) + ": int" : "";
            const auto variable = localSymbolKind == SymbolKind::VARIABLE ? "let " + std::string(identifier) + " = 0\n" : "";
            const auto body = variable + std::string(identifier) + "\n";
            accessModules.push_back(parseModule("func inspect(" + parameter + ") {\n" + (nestedBlock ? "{\n" + body + "}\n" : body) + "}\n", config));
            const auto& function = dynamic_cast<const FunctionDeclarationNode&>(*accessModules.back()->getTopIdentifierDeclarations().front());
            const auto& bodyBlock = *function.getBody().value();
            const auto& block = nestedBlock ? dynamic_cast<const BlockStatementNode&>(*bodyBlock.getStatements().front()) : bodyBlock;
            const auto& statement = dynamic_cast<const ExpressionStatementNode&>(*block.getStatements().back());
            const auto& expression = dynamic_cast<const IdentifierLikeExpressionNode&>(statement.getExpression());
            auto& context = analyzer->context;
            const auto* parent = context.getScopeByAstNode(accessor.empty() ? static_cast<const AstNode*>(module.get()) : findClass(accessor));
            context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, parent, &function));
            if (localSymbolKind == SymbolKind::PARAMETER) {
                EXPECT_TRUE(context.currentScope().declare(Symbol(SymbolKind::PARAMETER, SymbolAccessModifier::PUBLIC, identifier, function.getParameters().front().get())));
            }
            if (nestedBlock) context.pushScope(std::make_unique<Scope>(ScopeKind::BLOCK, &context.currentScope(), &block));
            if (localSymbolKind == SymbolKind::VARIABLE) {
                const auto& declaration = dynamic_cast<const VariableDeclarationStatementNode&>(*block.getStatements().front()).getVariableDeclaration();
                EXPECT_TRUE(context.currentScope().declare(Symbol(SymbolKind::VARIABLE, SymbolAccessModifier::PUBLIC, identifier, &declaration)));
            }
            const bool allowed = analyzer->checkAccessModifier(expression);
            if (nestedBlock) context.popScope();
            context.popScope();
            return allowed;
        }
    };

    TEST_F(SemanticAnalyzerAccessTest, AllowsImplicitThisAccessWithinTheDeclaringClass) {
        for (const auto member : { "publicMember", "protectedMember", "privateMember", "protectedInstanceMethod", "protectedMethod" }) {
            SCOPED_TRACE(member);
            EXPECT_TRUE(canAccessIdentifier("Base", member));
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, AllowsInheritedProtectedMembersThroughImplicitThis) {
        for (const auto accessor : { "Derived", "Descendant" }) {
            SCOPED_TRACE(accessor);
            for (const auto member : { "publicMember", "protectedMember", "protectedInstanceMethod", "protectedMethod" }) {
                SCOPED_TRACE(member);
                EXPECT_TRUE(canAccessIdentifier(accessor, member));
            }
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, DeniesInheritedPrivateMembersBeforeLookingUpModuleVariables) {
        EXPECT_FALSE(canAccessIdentifier("Derived", "privateMember"));
        EXPECT_FALSE(canAccessIdentifier("Descendant", "privateMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, LocalVariablesAndParametersHideInheritedPrivateMembers) {
        for (const auto localSymbolKind : { SymbolKind::VARIABLE, SymbolKind::PARAMETER }) {
            for (const bool nestedBlock : { false, true }) {
                SCOPED_TRACE(nestedBlock);
                EXPECT_TRUE(canAccessIdentifier("Derived", "privateMember", localSymbolKind, nestedBlock));
            }
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, ResolvesClassSymbolsBeforeInheritedMembersForImplicitThis) {
        EXPECT_TRUE(canAccessIdentifier("PublicShadow", "privateMember"));
        EXPECT_TRUE(canAccessIdentifier("PrivateShadow", "protectedMember"));
        EXPECT_TRUE(canAccessIdentifier("GenericPrivateShadow", "privateMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, DefersIdentifierResolutionErrorsToOtherChecks) {
        EXPECT_TRUE(canAccessIdentifier("", "privateMember"));
        EXPECT_TRUE(canAccessIdentifier("", "protectedMember"));
        EXPECT_TRUE(canAccessIdentifier("Derived", "missingMember"));
        EXPECT_TRUE(canAccessIdentifier("MissingBase", "missingMember"));
        EXPECT_TRUE(canAccessIdentifier("CycleA", "missingMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, ChecksImplicitThisBeforeTheCurrentClassTypeAndBaseScopeAreRegistered) {
        prepareUnregisteredDerivedScope();
        EXPECT_TRUE(canAccessIdentifier("Derived", "protectedMember"));
        EXPECT_TRUE(canAccessIdentifier("Derived", "protectedInstanceMethod"));
        EXPECT_TRUE(canAccessIdentifier("Derived", "protectedMethod"));
        EXPECT_FALSE(canAccessIdentifier("Derived", "privateMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, AllowsProtectedMembersWithinDeclaringAndDerivedClasses) {
        for (const auto accessor : { "Base", "Derived", "Descendant" }) {
            SCOPED_TRACE(accessor);
            EXPECT_TRUE(canAccess(accessor, accessor, "protectedMember"));
            EXPECT_TRUE(canAccess(accessor, accessor, "protectedInstanceMethod"));
            EXPECT_TRUE(canAccess("Base", accessor, "protectedMethod", true));
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, DeniesProtectedInstanceMembersThroughBaseOrSiblingInstances) {
        for (const auto member : { "protectedMember", "protectedInstanceMethod" }) {
            SCOPED_TRACE(member);
            EXPECT_FALSE(canAccess("Base", "Derived", member));
            EXPECT_FALSE(canAccess("Base", "Descendant", member));
            EXPECT_FALSE(canAccess("Derived", "Descendant", member));
            EXPECT_FALSE(canAccess("GenericShadow", "Derived", member));
            EXPECT_TRUE(canAccess("Descendant", "Derived", member));
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, AllowsProtectedInstanceMembersThroughThisAndSuper) {
        for (const auto member : { "protectedMember", "protectedInstanceMethod" }) {
            SCOPED_TRACE(member);
            EXPECT_TRUE(canAccess("Derived", "Derived", member, false, "this"));
            EXPECT_TRUE(canAccess("Descendant", "Descendant", member, false, "this"));
            EXPECT_TRUE(canAccess("Base", "Derived", member, false, "super"));
            EXPECT_TRUE(canAccess("Derived", "Descendant", member, false, "super"));
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, AllowsProtectedStaticMethodsThroughBaseAndSiblingInstances) {
        for (const auto receiver : { "Base", "GenericShadow" }) {
            SCOPED_TRACE(receiver);
            for (const bool typeExpression : { false, true }) {
                SCOPED_TRACE(typeExpression);
                EXPECT_TRUE(canAccess(receiver, "Derived", "protectedMethod", typeExpression));
                EXPECT_FALSE(canAccess(receiver, "Unrelated", "protectedMethod", typeExpression));
                EXPECT_FALSE(canAccess(receiver, "", "protectedMethod", typeExpression));
            }
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, DeniesProtectedMembersOutsideTheirHierarchy) {
        for (const auto accessor : { "Unrelated", "" }) {
            SCOPED_TRACE(accessor);
            EXPECT_FALSE(canAccess("Base", accessor, "protectedMember"));
            EXPECT_FALSE(canAccess("Base", accessor, "protectedMethod", true));
            EXPECT_FALSE(canAccess("Base", accessor, "protectedMember", false, "super"));
            EXPECT_FALSE(canAccess("Base", accessor, "protectedInstanceMethod", false, "super"));
        }
    }

    TEST_F(SemanticAnalyzerAccessTest, ChecksInheritedMembersAgainstTheirDeclaringClass) {
        EXPECT_TRUE(canAccess("Descendant", "Base", "protectedMember"));
        EXPECT_TRUE(canAccess("Descendant", "Derived", "protectedMember"));
        EXPECT_TRUE(canAccess("Descendant", "Descendant", "protectedMethod", true));
        EXPECT_FALSE(canAccess("Descendant", "Unrelated", "protectedMember"));
        EXPECT_FALSE(canAccess("Descendant", "", "protectedMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, PreservesPublicAndPrivateAccessRules) {
        EXPECT_TRUE(canAccess("Base", "", "publicMember"));
        EXPECT_TRUE(canAccess("Descendant", "", "publicMember"));
        EXPECT_TRUE(canAccess("Base", "Base", "privateMember"));
        EXPECT_TRUE(canAccess("Descendant", "Base", "privateMember"));
        EXPECT_FALSE(canAccess("Base", "Derived", "privateMember"));
        EXPECT_FALSE(canAccess("Base", "", "privateMember"));
        EXPECT_FALSE(canAccess("Descendant", "Descendant", "privateMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, DoesNotGrantProtectedAccessForMissingOrCyclicBaseTypes) {
        EXPECT_FALSE(canAccess("Base", "MissingBase", "protectedMember"));
        EXPECT_FALSE(canAccess("Base", "CycleA", "protectedMember"));
        EXPECT_TRUE(canAccess("CycleA", "Unrelated", "missingMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, ChecksDeclarationsBeforeTheirScopesAreBuilt) {
        leaveBaseScopeUnbuilt();
        EXPECT_FALSE(canAccess("Base", "", "protectedMember"));
        EXPECT_FALSE(canAccess("Base", "", "protectedMethod", true));
        EXPECT_FALSE(canAccess("Base", "", "privateMember"));
        EXPECT_TRUE(canAccess("Base", "", "publicMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, ChecksProtectedReceiversBeforeTheBaseScopeIsBuilt) {
        prepareDerivedScopeWithoutBaseScope();

        for (const auto member : { "protectedMember", "protectedInstanceMethod" }) {
            SCOPED_TRACE(member);
            EXPECT_FALSE(canAccess("Base", "Derived", member));
            EXPECT_TRUE(canAccess("Derived", "Derived", member));
            EXPECT_TRUE(canAccess("Base", "Derived", member, false, "super"));
        }
        EXPECT_TRUE(canAccess("Base", "Derived", "protectedMethod"));
        EXPECT_TRUE(canAccess("Base", "Derived", "protectedMethod", true));
    }

    TEST_F(SemanticAnalyzerAccessTest, UsesAccessModifierOfTheNearestMemberDeclaration) {
        EXPECT_TRUE(canAccess("PublicShadow", "", "protectedMember"));
        EXPECT_TRUE(canAccess("PrivateShadow", "PrivateShadow", "protectedMember"));
        EXPECT_FALSE(canAccess("PrivateShadow", "", "protectedMember"));
        EXPECT_FALSE(canAccess("PrivateShadow", "Base", "protectedMember"));
    }

    TEST_F(SemanticAnalyzerAccessTest, GenericParametersDoNotHideInheritedProtectedMembers) {
        EXPECT_FALSE(canAccess("GenericShadow", "", "protectedMember"));
        EXPECT_FALSE(canAccess("GenericShadow", "Unrelated", "protectedMember"));
        EXPECT_TRUE(canAccess("GenericShadow", "GenericShadow", "protectedMember"));
    }

    TEST(SemanticAnalyzerTest, AcceptsMatchingGenericArgumentCounts) {
        constexpr std::string_view source = R"(
class Box<T> {}
type BoxAlias = Box<int>
type Identity<T> = T
)";
        const auto config = makeConfig("generic_arguments.vnl");
        auto module = parseModule(source, config);

        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
    }

    TEST(SemanticAnalyzerTest, RejectsGenericArgumentCountMismatchWithoutCheckingArguments) {
        constexpr std::string_view source = R"(
class Box<T> {}
type MissingArgument = Box
type ExtraArgument = Box<Missing, Missing>
)";
        const auto config = makeConfig("generic_argument_errors.vnl");
        auto module = parseModule(source, config);

        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 2);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Generic argument count mismatch: expected 1, got 0");
        EXPECT_EQ(result.getErrors()[1].getMessage(), "Generic argument count mismatch: expected 1, got 2");
    }

    TEST(SemanticAnalyzerTest, DoesNotCheckGenericArgumentsOfInvalidTypeReferences) {
        constexpr std::string_view source = R"(
type Invalid = Missing<Unknown>
)";
        const auto config = makeConfig("invalid_generic_type.vnl");
        auto module = parseModule(source, config);

        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Use of undeclared type 'Missing'");
    }

    TEST(SemanticAnalyzerTest, AcceptsPrimitiveTypeExpressions) {
        constexpr std::string_view source = R"(
func test() {
    let value = int
}
)";
        const auto config = makeConfig("primitive_type_expression.vnl");
        auto module = parseModule(source, config);

        const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        SemanticAnalyzer analyzer(*module, imports);
        const auto result = analyzer.analyze(config);

        ASSERT_FALSE(result.hasErrors());
    }

    TEST(SemanticContextTest, RetainsPoppedScopesAndTheirParents) {
        const auto config = makeConfig("scopes.vnl");
        const auto module = parseModule("class Sample {}", config);
        const auto* classDeclaration = module->getTopIdentifierDeclarations().front().get();
        SemanticContext context;

        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, module.get()));
        const auto* moduleScope = &context.currentScope();
        ASSERT_TRUE(context.currentScope().declare(Symbol(SymbolKind::CLASS, SymbolAccessModifier::PUBLIC, "Sample", classDeclaration)));
        context.pushScope(std::make_unique<Scope>(ScopeKind::CLASS, moduleScope, classDeclaration));
        const auto* classScope = &context.currentScope();

        EXPECT_EQ(context.getScopeByAstNode(module.get()), moduleScope);
        EXPECT_EQ(context.getScopeByAstNode(classDeclaration), classScope);
        EXPECT_EQ(context.getScopeByAstNode(nullptr), nullptr);

        context.popScope();
        EXPECT_EQ(&context.currentScope(), moduleScope);
        EXPECT_EQ(context.getScopeByAstNode(classDeclaration), classScope);
        context.popScope();

        ASSERT_EQ(context.getScopeByAstNode(module.get()), moduleScope);
        ASSERT_EQ(context.getScopeByAstNode(classDeclaration), classScope);
        EXPECT_EQ(classScope->findParent(), moduleScope);
        const auto* symbol = classScope->lookup("Sample");
        ASSERT_NE(symbol, nullptr);
        EXPECT_EQ(symbol->getLocalNode(), classDeclaration);
    }

    TEST(SemanticContextTest, RetainsLocalAndImportedScopesSeparately) {
        const auto config = makeConfig("scopes.vnl");
        const auto module = parseModule("", config);
        const ImportedModule importedModule("api", {});
        const ImportedModule anotherModule("other", {});
        SemanticContext context;

        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, module.get()));
        const auto* localScope = &context.currentScope();
        EXPECT_EQ(localScope->getOrigin(), ScopeOrigin::LOCAL);
        EXPECT_EQ(localScope->getLocalNode(), module.get());
        EXPECT_EQ(localScope->getImportedNode(), nullptr);

        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, &importedModule));
        const auto* importedScope = &context.currentScope();
        EXPECT_EQ(importedScope->getOrigin(), ScopeOrigin::IMPORTED);
        EXPECT_EQ(importedScope->getLocalNode(), nullptr);
        EXPECT_EQ(importedScope->getImportedNode(), &importedModule);
        EXPECT_EQ(context.getScopeByAstNode(nullptr), nullptr);
        EXPECT_EQ(context.getScopeByImportedNode(nullptr), nullptr);
        EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
        EXPECT_EQ(context.getScopeByAstNode(module.get()), localScope);
        EXPECT_EQ(&context.getOrCreateImportedScope(ScopeKind::MODULE, nullptr, importedModule), importedScope);

        context.popScope();
        EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
        EXPECT_EQ(&context.getOrCreateImportedScope(ScopeKind::MODULE, nullptr, importedModule), importedScope);
        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, &anotherModule));
        const auto* anotherScope = &context.currentScope();
        context.popScope();
        context.popScope();

        EXPECT_EQ(context.getScopeByAstNode(module.get()), localScope);
        EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
        EXPECT_EQ(context.getScopeByImportedNode(&anotherModule), anotherScope);
        auto importedScopes = context.takeImportedScopeMap();
        ASSERT_EQ(importedScopes.size(), 2);
        EXPECT_EQ(importedScopes.at(&importedModule).get(), importedScope);
        EXPECT_EQ(importedScopes.at(&anotherModule).get(), anotherScope);
        auto localScopes = context.takeLocalScopeMap();
        ASSERT_EQ(localScopes.size(), 1);
        EXPECT_EQ(localScopes.at(module.get()).get(), localScope);
    }

    class SemanticAnalyzerImportTest : public testing::Test {
    protected:
        std::filesystem::path testDirectory;
        Config config = makeConfig("imports.vnl");
        std::shared_ptr<const ModuleNode> module;
        std::optional<CollectionResult> collectionResult;

        void SetUp() override {
            const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
            testDirectory = std::filesystem::temp_directory_path() / ("vnlctest-import-" + std::string(testInfo->name()));
            std::filesystem::remove_all(testDirectory);
            config.dependencyPackageRootPaths.emplace("pkg", testDirectory / "dependency_source");
            config.dependencyPackageRootPaths.emplace("extra", testDirectory / "another_source");
            writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"count":{"category":"let","type":"int"}})");
            writeFile("dependency_source/sub/other.vni", R"({"flag":{"category":"let","type":"bool"}})");
            writeFile("dependency_source/sub/second.vni", R"({"caption":{"category":"let","type":"string"}})");
            writeFile("another_source/tools.vni", R"({"enabled":{"category":"let","type":"bool"}})");
        }

        void TearDown() override {
            std::error_code error;
            std::filesystem::remove_all(testDirectory, error);
        }

        void writeFile(const std::filesystem::path& relativePath, std::string_view contents) const {
            const auto path = testDirectory / relativePath;
            std::filesystem::create_directories(path.parent_path());
            std::ofstream output(path);
            output << contents;
        }

        void writeScopedModule() const {
            writeFile("dependency_source/api.vni", R"({
    "value": {"category": "let", "type": "int"},
    "run": {"category": "func", "returnType": "void", "native": false,
        "parameters": {"amount": {"category": "parameter", "type": "int"}}},
    "Box": {"category": "class", "genericParameters": ["T"], "baseClass": null, "implementedInterfaces": [], "final": false,
        "properties": {
            "visible": {"category": "property", "type": "T", "static": false, "accessModifier": "public"},
            "secret": {"category": "property", "type": "int", "static": true, "accessModifier": "private"}},
        "methods": {"apply": {"category": "method", "returnType": "T", "native": false, "static": false, "accessModifier": "protected",
            "parameters": {"input": {"category": "parameter", "type": "T"}}}}},
    "Readable": {"category": "interface", "genericParameters": ["R"],
        "methods": {"read": {"category": "method", "returnType": "R", "native": false, "static": false, "accessModifier": "public",
            "parameters": {"input": {"category": "parameter", "type": "R"}}}}},
    "State": {"category": "enum", "genericParameters": ["E"],
        "members": {"Ready": {"category": "enummember", "associatedValues": {"payload": {"category": "enumvalue", "type": "E"}}}}},
    "Alias": {"category": "typealias", "genericParameters": ["A"], "originalType": "A"},
    "External": {"category": "imported", "source": "extra.tools.enabled"},
    "method": {"category": "method", "returnType": "void", "native": false, "static": true, "accessModifier": "public",
        "parameters": {"amount": {"category": "parameter", "type": "int"}}},
    "Ready": {"category": "enummember", "associatedValues": {"payload": {"category": "enumvalue", "type": "int"}}},
    "property": {"category": "property", "type": "int", "static": true, "accessModifier": "public"},
    "parameter": {"category": "parameter", "type": "int"}
})");
        }

        void prepareImports(std::string_view source) {
            collectionResult = collectModule(source, config);
            EXPECT_TRUE(collectionResult->getErrors().empty());
        }

        const ImportedPackage* getImportedPackageByName(std::string_view name) const {
            const auto& imports = collectionResult->getImports();
            const auto it = imports.find(std::string(name));
            return it == imports.end() ? nullptr : it->second.get();
        }

        SemanticResult analyze(std::string_view source, std::optional<std::string_view> importSource = std::nullopt) {
            prepareImports(importSource.value_or(source));
            const auto& imports = collectionResult->getImports();
            module = parseModule(source, config);
            SemanticAnalyzer analyzer(*module, imports);
            return analyzer.analyze(config);
        }

        template <typename Callback> void withAliasType(std::string_view source, Callback&& callback) {
            prepareImports(source);
            module = parseModule(source, config);
            const auto& imports = collectionResult->getImports();
            SemanticAnalyzer analyzer(*module, imports);
            auto& context = analyzer.context;
            context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, module.get()));
            for (const auto& importDecl : module->getImportDeclarations()) {
                analyzer.checkImport(*importDecl, config);
            }
            const TypeAliasDeclarationNode* typeAlias = nullptr;
            for (const auto& topIdentifierDecl : module->getTopIdentifierDeclarations()) {
                DeclarationNode* declaration = topIdentifierDecl.get();
                if (auto* valueDecl = dynamic_cast<ValueDeclarationNode*>(declaration)) {
                    context.currentScope().declare(
                        Symbol(SymbolKind::VARIABLE, static_cast<SymbolAccessModifier>(valueDecl->getAccessModifier()), valueDecl->getName().getIdentifierString(), valueDecl)
                    );
                    analyzer.checkValueDeclaration(*valueDecl);
                } else if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(declaration)) {
                    context.currentScope().declare(
                        Symbol(SymbolKind::FUNCTION, static_cast<SymbolAccessModifier>(funcDecl->getAccessModifier()), funcDecl->getName().getIdentifierString(), funcDecl)
                    );
                    analyzer.checkFunctionDeclaration(*funcDecl);
                } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declaration)) {
                    context.currentScope().declare(Symbol(SymbolKind::CLASS, SymbolAccessModifier::PUBLIC, classDecl->getName().getIdentifierString(), classDecl));
                    analyzer.checkClassDeclaration(*classDecl, config);
                } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declaration)) {
                    context.currentScope().declare(Symbol(SymbolKind::INTERFACE, SymbolAccessModifier::PUBLIC, interfaceDecl->getName().getIdentifierString(), interfaceDecl));
                    analyzer.checkInterfaceDeclaration(*interfaceDecl, config);
                } else if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declaration)) {
                    context.currentScope().declare(Symbol(SymbolKind::ENUM, SymbolAccessModifier::PUBLIC, enumDecl->getName().getIdentifierString(), enumDecl));
                    analyzer.checkEnumDeclaration(*enumDecl, config);
                } else if (auto* typeAliasDecl = dynamic_cast<TypeAliasDeclarationNode*>(declaration)) {
                    context.currentScope().declare(Symbol(SymbolKind::TYPE_ALIAS, SymbolAccessModifier::PUBLIC, typeAliasDecl->getAliasName().getIdentifierString(), typeAliasDecl));
                    analyzer.checkTypeAliasDeclaration(*typeAliasDecl, config);
                    typeAlias = typeAliasDecl;
                }
            }
            if (typeAlias == nullptr) {
                context.popScope();
                return;
            }
            context.pushScope(std::make_unique<Scope>(ScopeKind::TYPE_ALIAS, &context.currentScope(), typeAlias));
            for (const auto& genericParameter : typeAlias->getGenericParameterNames()) {
                context.currentScope().declare(Symbol(SymbolKind::GENERIC_PARAMETER, SymbolAccessModifier::PUBLIC, genericParameter->getIdentifierString(), typeAlias));
            }
            callback(analyzer, *typeAlias);
            context.popScope();
            context.popScope();
        }

        std::string getFullTypeNameByAlias(std::string_view source) {
            std::string fullName;
            withAliasType(source, [&fullName](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
                fullName = analyzer.getFullTypeNameByTypeReferenceNode(typeAlias.getOriginalType());
            });
            return fullName;
        }

        SemanticContext& semanticContext(SemanticAnalyzer& analyzer) const {
            return analyzer.context;
        }

        const Type* checkedType(SemanticAnalyzer& analyzer, const TypeReferenceNode& type) const {
            return analyzer.checkType(type);
        }

        const ImportedItem* findImportedNode(const SemanticResult& result, std::string_view name) const {
            const auto* scope = result.getScopeByAstNode(*module);
            EXPECT_NE(scope, nullptr);
            if (scope == nullptr) return nullptr;
            const auto* symbol = scope->lookupLocal(name);
            return symbol != nullptr ? symbol->getImportedNode() : nullptr;
        }
    };

    TEST_F(SemanticAnalyzerImportTest, PrefixesImportedTypesWithTheirCanonicalPaths) {
        writeScopedModule();
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.Box as ImportedBox\ntype Result = ImportedBox\n"), "pkg.api.Box");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api as api\ntype Result = api.Box\n"), "pkg.api.Box");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg as library\ntype Result = library.api.Box\n"), "pkg.api.Box");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.*\ntype Result = Box\n"), "pkg.api.Box");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.State as State\ntype Result = State.Ready\n"), "pkg.api.State.Ready");
    }

    TEST_F(SemanticAnalyzerImportTest, PrefixesGenericArgumentsAndDesugarsOptionalImportedTypes) {
        writeScopedModule();
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api as api\ntype Result = api.Box<api.Readable>\n"), "pkg.api.Box<pkg.api.Readable>");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.*\ntype Result = Box<Readable>\n"), "pkg.api.Box<pkg.api.Readable>");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.Box as ImportedBox\ntype Result = ImportedBox<int?>\n"), "pkg.api.Box<vanillang.typesystem.Optional<int>>");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.Box as ImportedBox\ntype Result<T> = ImportedBox<T>\n"), "pkg.api.Box<T>");
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.Box as ImportedBox\ntype Result = ImportedBox?\n"), "vanillang.typesystem.Optional<pkg.api.Box>");
        EXPECT_EQ(
            getFullTypeNameByAlias("import pkg.api.Box as ImportedBox\ntype Result = ImportedBox<int?>?\n"),
            "vanillang.typesystem.Optional<pkg.api.Box<vanillang.typesystem.Optional<int>>>"
        );
    }

    TEST_F(SemanticAnalyzerImportTest, BuildsLocalFullTypeNames) {
        const auto classFullName = getFullTypeNameByAlias("class Box {}\ntype Result = Box\n");
        EXPECT_EQ(classFullName, std::string(module->getFullName()) + ".Box");

        const auto interfaceFullName = getFullTypeNameByAlias("interface Readable {}\ntype Result = Readable\n");
        EXPECT_EQ(interfaceFullName, std::string(module->getFullName()) + ".Readable");

        const auto enumMemberFullName = getFullTypeNameByAlias("enum State {\n    Ready\n}\ntype Result = State.Ready\n");
        EXPECT_EQ(enumMemberFullName, std::string(module->getFullName()) + ".State.Ready");

        const auto typeAliasFullName = getFullTypeNameByAlias("type Alias = int\ntype Result = Alias\n");
        EXPECT_EQ(typeAliasFullName, std::string(module->getFullName()) + ".Alias");
    }

    TEST_F(SemanticAnalyzerImportTest, BuildsLocalGenericAndOptionalFullTypeNames) {
        const auto genericFullName = getFullTypeNameByAlias("class Box<T> {}\ntype Result<T> = Box<T?>\n");
        EXPECT_EQ(genericFullName, std::string(module->getFullName()) + ".Box<vanillang.typesystem.Optional<T>>");

        const auto optionalCustomizedFullName = getFullTypeNameByAlias("class Box<T> {}\ninterface Readable {}\ntype Result = Box<Readable?>\n");
        EXPECT_EQ(optionalCustomizedFullName, std::string(module->getFullName()) + ".Box<vanillang.typesystem.Optional<" + std::string(module->getFullName()) + ".Readable>>");

        EXPECT_EQ(getFullTypeNameByAlias("type Result = int?\n"), "vanillang.typesystem.Optional<int>");
    }

    TEST_F(SemanticAnalyzerImportTest, MapsPrimitiveTypeReferenceNodesToSharedTypes) {
        withAliasType("type Result = int\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            const auto* type = context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType());
            ASSERT_NE(type, nullptr);
            EXPECT_EQ(type, PrimitiveType::intType());
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), type);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersOptionalPrimitiveTypesWithTheirOriginalArguments) {
        for (const auto* primitiveType : { PrimitiveType::byteType(),
                                           PrimitiveType::shortType(),
                                           PrimitiveType::intType(),
                                           PrimitiveType::longType(),
                                           PrimitiveType::floatType(),
                                           PrimitiveType::doubleType(),
                                           PrimitiveType::booleanType(),
                                           PrimitiveType::stringType() }) {
            const auto primitiveName = std::string(primitiveType->getFullTypeName());
            SCOPED_TRACE(primitiveName);
            withAliasType("type Result = " + primitiveName + "?\n", [this, primitiveType, &primitiveName](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
                auto& context = semanticContext(analyzer);
                ASSERT_TRUE(context.getErrors().empty());
                const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
                ASSERT_NE(optionalType, nullptr);
                EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
                EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<" + primitiveName + ">");
                ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
                EXPECT_EQ(optionalType->getGenericArguments().front(), primitiveType);
                EXPECT_EQ(context.getCustomizedTypeByFullTypeName(std::string(optionalType->getFullTypeName())), optionalType);
                EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), optionalType);
            });
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersNestedOptionalLocalTypesWithTheirOriginalArguments) {
        withAliasType("class Box<T> {}\ntype Result = Box<int?>?\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            ASSERT_TRUE(context.getErrors().empty());
            const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
            ASSERT_NE(optionalType, nullptr);
            EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
            const auto boxName = std::string(module->getFullName()) + ".Box<vanillang.typesystem.Optional<int>>";
            EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<" + boxName + ">");
            ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
            const auto* boxType = dynamic_cast<const CustomizedType*>(optionalType->getGenericArguments().front());
            ASSERT_NE(boxType, nullptr);
            EXPECT_EQ(boxType->getFullTypeName(), boxName);
            EXPECT_EQ(boxType->getCustomizedKind(), CustomizedTypeKind::CLASS);
            EXPECT_EQ(boxType->getOrigin(), CustomizedTypeOrigin::LOCAL);
            EXPECT_EQ(boxType->getLocalNode(), module->getTopIdentifierDeclarations().front().get());
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName(boxName), boxType);
            ASSERT_EQ(boxType->getGenericArguments().size(), 1);
            const auto* optionalIntType = dynamic_cast<const CustomizedType*>(boxType->getGenericArguments().front());
            ASSERT_NE(optionalIntType, nullptr);
            EXPECT_EQ(optionalIntType->getCustomizedKind(), CustomizedTypeKind::ENUM);
            EXPECT_EQ(optionalIntType->getFullTypeName(), "vanillang.typesystem.Optional<int>");
            const auto& originalType = dynamic_cast<const CustomizedTypeReferenceNode&>(typeAlias.getOriginalType());
            EXPECT_EQ(context.getTypeByTypeReferenceNode(originalType.getGenericArguments().front().get()), optionalIntType);
            ASSERT_EQ(optionalIntType->getGenericArguments().size(), 1);
            EXPECT_EQ(optionalIntType->getGenericArguments().front(), PrimitiveType::intType());
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName(std::string(optionalType->getFullTypeName())), optionalType);
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), optionalType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersOptionalImportedTypesWithTheirOriginalDeclarations) {
        writeScopedModule();
        withAliasType("import pkg.api.Box as ImportedBox\ntype Result = ImportedBox<int>?\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            ASSERT_TRUE(context.getErrors().empty());
            const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
            ASSERT_NE(optionalType, nullptr);
            EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
            EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<pkg.api.Box<int>>");
            ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
            const auto* boxType = dynamic_cast<const CustomizedType*>(optionalType->getGenericArguments().front());
            ASSERT_NE(boxType, nullptr);
            EXPECT_EQ(boxType->getFullTypeName(), "pkg.api.Box<int>");
            EXPECT_EQ(boxType->getCustomizedKind(), CustomizedTypeKind::CLASS);
            EXPECT_EQ(boxType->getOrigin(), CustomizedTypeOrigin::IMPORTED);
            const auto* symbol = context.currentScope().lookup("ImportedBox");
            ASSERT_NE(symbol, nullptr);
            EXPECT_EQ(boxType->getImportedNode(), symbol->getImportedNode());
            ASSERT_EQ(boxType->getGenericArguments().size(), 1);
            EXPECT_EQ(boxType->getGenericArguments().front(), PrimitiveType::intType());
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName("pkg.api.Box<int>"), boxType);
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), optionalType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersOptionalGenericParametersWithTheirOriginalDeclarations) {
        withAliasType("type Result<T> = T?\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            ASSERT_TRUE(context.getErrors().empty());
            const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType()));
            ASSERT_NE(optionalType, nullptr);
            EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
            EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<T>");
            ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
            const auto* genericType = dynamic_cast<const CustomizedType*>(optionalType->getGenericArguments().front());
            ASSERT_NE(genericType, nullptr);
            EXPECT_EQ(genericType->getCustomizedKind(), CustomizedTypeKind::GENERIC_PARAMETER);
            EXPECT_EQ(genericType->getFullTypeName(), "T");
            EXPECT_EQ(genericType->getLocalNode(), &typeAlias);
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName("T"), genericType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RetainsSharedOptionalTypesAndTheirOriginalTypesInAnalysisResults) {
        const auto result = analyze("class Box<T> {}\ntype Original = Box<int>\ntype First = Box<int>?\ntype Second = Box<int>?\n");
        ASSERT_FALSE(result.hasErrors());
        const auto& declarations = module->getTopIdentifierDeclarations();
        const auto& originalAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*declarations.at(1));
        const auto& firstAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*declarations.at(2));
        const auto& secondAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*declarations.at(3));
        const auto* originalType = result.getTypeByTypeReferenceNode(&originalAlias.getOriginalType());
        const auto* optionalType = dynamic_cast<const CustomizedType*>(result.getTypeByTypeReferenceNode(&firstAlias.getOriginalType()));
        ASSERT_NE(originalType, nullptr);
        ASSERT_NE(optionalType, nullptr);
        EXPECT_NE(originalType, optionalType);
        EXPECT_EQ(result.getTypeByTypeReferenceNode(&secondAlias.getOriginalType()), optionalType);
        ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
        EXPECT_EQ(optionalType->getGenericArguments().front(), originalType);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName(std::string(originalType->getFullTypeName())), originalType);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName(std::string(optionalType->getFullTypeName())), optionalType);
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotRegisterOptionalTypesWithUnresolvedOriginalTypes) {
        const auto result = analyze("class Box<T> {}\ntype MissingOptional = Missing?\ntype MissingArgument = Box<Missing?>\ntype MissingOptionalArgument = Box<Missing?>?\n");
        ASSERT_EQ(result.getErrors().size(), 3);
        for (const auto& error : result.getErrors()) {
            EXPECT_EQ(error.getMessage(), "Use of undeclared type 'Missing'");
        }
        for (std::size_t index = 1; index < module->getTopIdentifierDeclarations().size(); ++index) {
            const auto& alias = dynamic_cast<const TypeAliasDeclarationNode&>(*module->getTopIdentifierDeclarations()[index]);
            EXPECT_EQ(result.getTypeByTypeReferenceNode(&alias.getOriginalType()), nullptr);
        }
        const auto boxName = std::string(module->getFullName()) + ".Box<vanillang.typesystem.Optional<Missing>>";
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName("vanillang.typesystem.Optional<Missing>"), nullptr);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName(boxName), nullptr);
        EXPECT_EQ(result.getCustomizedTypeByFullTypeName("vanillang.typesystem.Optional<" + boxName + ">"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, ReusesOptionalTypesAcrossSugarAndExplicitReferencesInEitherOrder) {
        config.dependencyPackageRootPaths.emplace("vanillang", testDirectory / "standard_library");
        writeFile("standard_library/typesystem.vni", R"({"Optional":{"category":"enum","genericParameters":["T"],"members":{}}})");
        for (const std::string_view firstType : { "int?", "Optional<int>" }) {
            SCOPED_TRACE(firstType);
            const std::string secondType = firstType == "int?" ? "Optional<int>" : "int?";
            withAliasType(
                "import vanillang.typesystem.Optional\ntype First = " + std::string(firstType) + "\ntype Second = " + secondType + "\n",
                [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& secondAlias) {
                    auto& context = semanticContext(analyzer);
                    ASSERT_TRUE(context.getErrors().empty());
                    const auto& firstAlias = dynamic_cast<const TypeAliasDeclarationNode&>(*module->getTopIdentifierDeclarations().front());
                    const auto* optionalType = dynamic_cast<const CustomizedType*>(context.getTypeByTypeReferenceNode(&firstAlias.getOriginalType()));
                    ASSERT_NE(optionalType, nullptr);
                    EXPECT_EQ(optionalType->getCustomizedKind(), CustomizedTypeKind::ENUM);
                    EXPECT_EQ(optionalType->getFullTypeName(), "vanillang.typesystem.Optional<int>");
                    EXPECT_EQ(context.getTypeByTypeReferenceNode(&secondAlias.getOriginalType()), optionalType);
                    EXPECT_EQ(context.getCustomizedTypeByFullTypeName("vanillang.typesystem.Optional<int>"), optionalType);
                    ASSERT_EQ(optionalType->getGenericArguments().size(), 1);
                    EXPECT_EQ(optionalType->getGenericArguments().front(), PrimitiveType::intType());
                }
            );
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersLocalCustomizedTypesAndReusesExistingPointers) {
        withAliasType("class Box<T> {}\ntype Result = Box<int>\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            const auto* resolvedType = context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType());
            const auto* customizedType = dynamic_cast<const CustomizedType*>(resolvedType);
            ASSERT_NE(customizedType, nullptr);
            EXPECT_EQ(customizedType->getCustomizedKind(), CustomizedTypeKind::CLASS);
            EXPECT_EQ(customizedType->getOrigin(), CustomizedTypeOrigin::LOCAL);
            EXPECT_EQ(customizedType->getFullTypeName(), std::string(module->getFullName()) + ".Box<int>");
            ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
            EXPECT_EQ(customizedType->getGenericArguments().front(), PrimitiveType::intType());
            EXPECT_EQ(customizedType->getLocalNode(), module->getTopIdentifierDeclarations().front().get());
            EXPECT_EQ(context.getCustomizedTypeByFullTypeName(std::string(customizedType->getFullTypeName())), customizedType);
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), customizedType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, ReusesCustomizedTypesAcrossDifferentTypeReferenceNodes) {
        withAliasType("class Box<T> {}\ntype First = Box<int>\ntype Second = Box<int>\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& secondAlias) {
            const TypeAliasDeclarationNode* firstAlias = nullptr;
            for (const auto& declaration : module->getTopIdentifierDeclarations()) {
                const auto* typeAlias = dynamic_cast<const TypeAliasDeclarationNode*>(declaration.get());
                if (typeAlias != nullptr && typeAlias->getAliasName().getIdentifierString() == "First") {
                    firstAlias = typeAlias;
                    break;
                }
            }
            ASSERT_NE(firstAlias, nullptr);
            auto& context = semanticContext(analyzer);
            EXPECT_EQ(context.getTypeByTypeReferenceNode(&firstAlias->getOriginalType()), context.getTypeByTypeReferenceNode(&secondAlias.getOriginalType()));
        });
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersAndReusesImportedCustomizedTypes) {
        writeScopedModule();
        withAliasType("import pkg.api.Box as ImportedBox\ntype Result = ImportedBox<int>\n", [this](SemanticAnalyzer& analyzer, const TypeAliasDeclarationNode& typeAlias) {
            auto& context = semanticContext(analyzer);
            const auto* resolvedType = context.getTypeByTypeReferenceNode(&typeAlias.getOriginalType());
            const auto* customizedType = dynamic_cast<const CustomizedType*>(resolvedType);
            ASSERT_NE(customizedType, nullptr);
            EXPECT_EQ(customizedType->getCustomizedKind(), CustomizedTypeKind::CLASS);
            EXPECT_EQ(customizedType->getOrigin(), CustomizedTypeOrigin::IMPORTED);
            EXPECT_EQ(customizedType->getFullTypeName(), "pkg.api.Box<int>");
            ASSERT_EQ(customizedType->getGenericArguments().size(), 1);
            EXPECT_EQ(customizedType->getGenericArguments().front(), PrimitiveType::intType());
            const auto* moduleScope = context.currentScope().findParent();
            ASSERT_NE(moduleScope, nullptr);
            const auto* symbol = moduleScope->lookupLocal("ImportedBox");
            ASSERT_NE(symbol, nullptr);
            EXPECT_EQ(customizedType->getImportedNode(), symbol->getImportedNode());
            EXPECT_EQ(checkedType(analyzer, typeAlias.getOriginalType()), customizedType);
        });
    }

    TEST_F(SemanticAnalyzerImportTest, BuildsImportedPackageModuleAndMemberScopes) {
        writeScopedModule();
        const auto result = analyze("import pkg as library\nlet localOnly = 0\nexport library\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* localScope = result.getScopeByAstNode(*module);
        ASSERT_NE(localScope, nullptr);
        EXPECT_EQ(localScope->getOrigin(), ScopeOrigin::LOCAL);
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* packageScope = result.getScopeByImportedNode(*package);
        ASSERT_NE(packageScope, nullptr);
        EXPECT_EQ(packageScope->getKind(), ScopeKind::PACKAGE);
        EXPECT_EQ(packageScope->getOrigin(), ScopeOrigin::IMPORTED);
        EXPECT_EQ(packageScope->getImportedNode(), package);
        EXPECT_EQ(packageScope->getLocalNode(), nullptr);
        EXPECT_EQ(packageScope->findParent(), nullptr);
        EXPECT_EQ(findImportedNode(result, "library"), package);

        const auto* api = package->getModuleByName("api");
        ASSERT_NE(api, nullptr);
        const auto* apiScope = result.getScopeByImportedNode(*api);
        ASSERT_NE(apiScope, nullptr);
        EXPECT_EQ(apiScope->getKind(), ScopeKind::MODULE);
        EXPECT_EQ(apiScope->findParent(), packageScope);
        const auto* apiSymbol = packageScope->lookupLocal("api");
        ASSERT_NE(apiSymbol, nullptr);
        EXPECT_EQ(apiSymbol->getKind(), SymbolKind::MODULE);
        EXPECT_EQ(apiSymbol->getImportedNode(), api);

        const auto expectChildScope = [&](const Scope& parent, std::string_view name, ScopeKind kind) -> const Scope* {
            const auto* symbol = parent.lookupLocal(name);
            EXPECT_NE(symbol, nullptr);
            if (symbol == nullptr) return nullptr;
            const auto* node = symbol->getImportedNode();
            EXPECT_NE(node, nullptr);
            if (node == nullptr) return nullptr;
            const auto* scope = result.getScopeByImportedNode(*node);
            EXPECT_NE(scope, nullptr);
            if (scope != nullptr) {
                EXPECT_EQ(scope->getKind(), kind);
                EXPECT_EQ(scope->getOrigin(), ScopeOrigin::IMPORTED);
                EXPECT_EQ(scope->getImportedNode(), node);
                EXPECT_EQ(scope->getLocalNode(), nullptr);
                EXPECT_EQ(scope->findParent(), &parent);
            }
            return scope;
        };
        const auto* subScope = expectChildScope(*packageScope, "sub", ScopeKind::PACKAGE);
        ASSERT_NE(subScope, nullptr);
        EXPECT_NE(expectChildScope(*subScope, "other", ScopeKind::MODULE), nullptr);

        const auto* boxScope = expectChildScope(*apiScope, "Box", ScopeKind::CLASS);
        ASSERT_NE(boxScope, nullptr);
        const auto* visible = boxScope->lookupLocal("visible");
        const auto* secret = boxScope->lookupLocal("secret");
        const auto* method = boxScope->lookupLocal("apply");
        ASSERT_NE(visible, nullptr);
        ASSERT_NE(secret, nullptr);
        ASSERT_NE(method, nullptr);
        EXPECT_EQ(visible->getKind(), SymbolKind::PROPERTY);
        EXPECT_EQ(visible->getAccessModifier(), SymbolAccessModifier::PUBLIC);
        EXPECT_EQ(secret->getAccessModifier(), SymbolAccessModifier::PRIVATE);
        EXPECT_EQ(method->getKind(), SymbolKind::METHOD);
        EXPECT_EQ(method->getAccessModifier(), SymbolAccessModifier::PROTECTED);
        EXPECT_EQ(result.getScopeByImportedNode(*visible->getImportedNode()), nullptr);
        const auto* methodScope = expectChildScope(*boxScope, "apply", ScopeKind::FUNCTION);
        ASSERT_NE(methodScope, nullptr);
        const auto* input = methodScope->lookupLocal("input");
        ASSERT_NE(input, nullptr);
        EXPECT_EQ(input->getKind(), SymbolKind::PARAMETER);
        EXPECT_EQ(input->getOrigin(), SymbolOrigin::IMPORTED);
        EXPECT_EQ(result.getScopeByImportedNode(*input->getImportedNode()), nullptr);
        const auto* generic = boxScope->lookupLocal("T");
        ASSERT_NE(generic, nullptr);
        EXPECT_EQ(generic->getKind(), SymbolKind::GENERIC_PARAMETER);
        EXPECT_EQ(generic->getImportedNode(), boxScope->getImportedNode());
        EXPECT_EQ(methodScope->lookup("T"), generic);
        EXPECT_EQ(methodScope->lookup("localOnly"), nullptr);

        const auto* interfaceScope = expectChildScope(*apiScope, "Readable", ScopeKind::INTERFACE);
        ASSERT_NE(interfaceScope, nullptr);
        const auto* readScope = expectChildScope(*interfaceScope, "read", ScopeKind::FUNCTION);
        ASSERT_NE(readScope, nullptr);
        EXPECT_NE(readScope->lookupLocal("input"), nullptr);
        EXPECT_NE(interfaceScope->lookupLocal("R"), nullptr);
        EXPECT_EQ(readScope->lookup("R"), interfaceScope->lookupLocal("R"));

        const auto* enumScope = expectChildScope(*apiScope, "State", ScopeKind::ENUM);
        ASSERT_NE(enumScope, nullptr);
        const auto* memberScope = expectChildScope(*enumScope, "Ready", ScopeKind::ENUM_MEMBER);
        ASSERT_NE(memberScope, nullptr);
        const auto* payload = memberScope->lookupLocal("payload");
        ASSERT_NE(payload, nullptr);
        EXPECT_EQ(payload->getKind(), SymbolKind::PROPERTY);
        EXPECT_EQ(payload->getOrigin(), SymbolOrigin::IMPORTED);
        EXPECT_EQ(result.getScopeByImportedNode(*payload->getImportedNode()), nullptr);
        EXPECT_NE(enumScope->lookupLocal("E"), nullptr);
        EXPECT_EQ(memberScope->lookup("E"), enumScope->lookupLocal("E"));

        const auto* aliasScope = expectChildScope(*apiScope, "Alias", ScopeKind::TYPE_ALIAS);
        ASSERT_NE(aliasScope, nullptr);
        EXPECT_NE(aliasScope->lookupLocal("A"), nullptr);
        const auto* functionScope = expectChildScope(*apiScope, "run", ScopeKind::FUNCTION);
        ASSERT_NE(functionScope, nullptr);
        EXPECT_NE(functionScope->lookupLocal("amount"), nullptr);
        const auto* external = apiScope->lookupLocal("External");
        ASSERT_NE(external, nullptr);
        EXPECT_EQ(external->getKind(), SymbolKind::IMPORT_ALIAS);
        EXPECT_EQ(result.getScopeByImportedNode(*external->getImportedNode()), nullptr);
        const auto* extraPackage = getImportedPackageByName("extra");
        ASSERT_NE(extraPackage, nullptr);
        EXPECT_EQ(result.getScopeByImportedNode(*extraPackage), nullptr);
        for (const auto name : { "api", "Box", "T", "apply", "input", "Ready", "payload", "extra", "External" }) {
            EXPECT_EQ(localScope->lookupLocal(name), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, CreatesScopesForDirectAndWildcardImportsOfScopedIdentifiers) {
        writeScopedModule();
        const std::vector<std::pair<std::string, ScopeKind>> scopedIdentifiers = {
            { "run", ScopeKind::FUNCTION },     { "Box", ScopeKind::CLASS },       { "Readable", ScopeKind::INTERFACE }, { "State", ScopeKind::ENUM },
            { "Alias", ScopeKind::TYPE_ALIAS }, { "method", ScopeKind::FUNCTION }, { "Ready", ScopeKind::ENUM_MEMBER },
        };
        for (const auto& [name, kind] : scopedIdentifiers) {
            SCOPED_TRACE(name);
            const auto result = analyze("import pkg.api." + name + " as selected\nexport selected\n");
            ASSERT_FALSE(result.hasErrors());
            const auto* node = findImportedNode(result, "selected");
            ASSERT_NE(node, nullptr);
            const auto* scope = result.getScopeByImportedNode(*node);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->getKind(), kind);
            EXPECT_EQ(scope->getImportedNode(), node);
            const auto* localScope = result.getScopeByAstNode(*module);
            ASSERT_NE(localScope, nullptr);
            EXPECT_EQ(localScope->lookupLocal(name), nullptr);
            EXPECT_EQ(localScope->lookupLocal("api"), nullptr);
            EXPECT_EQ(localScope->lookupLocal("pkg"), nullptr);
        }
        const auto result = analyze("import pkg.api.*\n");
        ASSERT_FALSE(result.hasErrors());
        for (const auto& [name, kind] : scopedIdentifiers) {
            const auto* node = findImportedNode(result, name);
            ASSERT_NE(node, nullptr);
            const auto* scope = result.getScopeByImportedNode(*node);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->getKind(), kind);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotCreateScopesForImportedValuesOrAliasesToValues) {
        writeScopedModule();
        for (const auto name : { "value", "property", "parameter", "External" }) {
            SCOPED_TRACE(name);
            const auto result = analyze(std::string("import pkg.api.") + name + "\n");
            ASSERT_FALSE(result.hasErrors());
            const auto* node = findImportedNode(result, name);
            ASSERT_NE(node, nullptr);
            EXPECT_EQ(result.getScopeByImportedNode(*node), nullptr);
            const auto* package = getImportedPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            EXPECT_EQ(result.getScopeByImportedNode(*package), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, ReusesImportedScopesAcrossAliasesAndImportOrders) {
        writeScopedModule();
        for (const auto source : {
                 "import pkg.api.Box as First\nimport pkg.api as api\nimport pkg as library\nimport pkg.api.Box as Second\nimport extra.tools as tools\n",
                 "import pkg as library\nimport pkg.api as api\nimport pkg.api.Box as First\nimport pkg.api.Box as Second\nimport extra.tools as tools\n",
                 "import pkg.api as api\nimport pkg.api.Box as First\nimport pkg as library\nimport pkg.api.Box as Second\nimport extra.tools as tools\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);
            ASSERT_FALSE(result.hasErrors());
            const auto* first = findImportedNode(result, "First");
            const auto* second = findImportedNode(result, "Second");
            const auto* api = findImportedNode(result, "api");
            const auto* library = findImportedNode(result, "library");
            const auto* tools = findImportedNode(result, "tools");
            ASSERT_NE(first, nullptr);
            ASSERT_EQ(first, second);
            ASSERT_NE(api, nullptr);
            ASSERT_NE(library, nullptr);
            ASSERT_NE(tools, nullptr);
            const auto* classScope = result.getScopeByImportedNode(*first);
            const auto* apiScope = result.getScopeByImportedNode(*api);
            const auto* packageScope = result.getScopeByImportedNode(*library);
            ASSERT_NE(classScope, nullptr);
            ASSERT_NE(apiScope, nullptr);
            ASSERT_NE(packageScope, nullptr);
            EXPECT_EQ(classScope, result.getScopeByImportedNode(*second));
            EXPECT_EQ(classScope->findParent(), apiScope);
            EXPECT_EQ(apiScope->findParent(), packageScope);
            const auto* method = classScope->lookupLocal("apply");
            ASSERT_NE(method, nullptr);
            const auto* methodScope = result.getScopeByImportedNode(*method->getImportedNode());
            ASSERT_NE(methodScope, nullptr);
            EXPECT_EQ(methodScope->findParent(), classScope);
            EXPECT_EQ(methodScope->lookup("T"), classScope->lookupLocal("T"));
            const auto* toolsScope = result.getScopeByImportedNode(*tools);
            ASSERT_NE(toolsScope, nullptr);
            EXPECT_NE(toolsScope->lookupLocal("enabled"), nullptr);
            EXPECT_NE(toolsScope, apiScope);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresScopesForModulesAcrossSeparateImports) {
        const auto result = analyze("import pkg.api as api\nimport pkg.sub.other.flag\n");
        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* packageScope = result.getScopeByImportedNode(*package);
        ASSERT_NE(packageScope, nullptr);
        const auto* sub = packageScope->lookupLocal("sub");
        ASSERT_NE(sub, nullptr);
        const auto* subScope = result.getScopeByImportedNode(*sub->getImportedNode());
        ASSERT_NE(subScope, nullptr);
        EXPECT_EQ(subScope->findParent(), packageScope);
        const auto* other = subScope->lookupLocal("other");
        ASSERT_NE(other, nullptr);
        const auto* otherScope = result.getScopeByImportedNode(*other->getImportedNode());
        ASSERT_NE(otherScope, nullptr);
        const auto* flag = otherScope->lookupLocal("flag");
        ASSERT_NE(flag, nullptr);
        EXPECT_EQ(flag->getImportedNode(), findImportedNode(result, "flag"));
        EXPECT_EQ(otherScope->findParent(), subScope);
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresScopesForIndirectModulesInAnImportedPackage) {
        writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"extra.types.Remote"}})");
        writeFile("another_source/types.vni", R"({"Remote":{"category":"typealias","genericParameters":["T"],"originalType":"T"}})");
        const auto result = analyze("import extra.tools as tools\nimport pkg.api.value\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedPackageByName("extra");
        ASSERT_NE(package, nullptr);
        const auto* packageScope = result.getScopeByImportedNode(*package);
        ASSERT_NE(packageScope, nullptr);
        const auto* types = packageScope->lookupLocal("types");
        ASSERT_NE(types, nullptr);
        const auto* typesScope = result.getScopeByImportedNode(*types->getImportedNode());
        ASSERT_NE(typesScope, nullptr);
        EXPECT_EQ(typesScope->findParent(), packageScope);
        const auto* remote = typesScope->lookupLocal("Remote");
        ASSERT_NE(remote, nullptr);
        const auto* remoteScope = result.getScopeByImportedNode(*remote->getImportedNode());
        ASSERT_NE(remoteScope, nullptr);
        EXPECT_EQ(remoteScope->findParent(), typesScope);
        EXPECT_NE(remoteScope->lookupLocal("T"), nullptr);
        const auto* tools = findImportedNode(result, "tools");
        ASSERT_NE(tools, nullptr);
        const auto* toolsScope = result.getScopeByImportedNode(*tools);
        ASSERT_NE(toolsScope, nullptr);
        EXPECT_EQ(toolsScope->findParent(), packageScope);
        const auto* localScope = result.getScopeByAstNode(*module);
        ASSERT_NE(localScope, nullptr);
        EXPECT_EQ(localScope->lookupLocal("types"), nullptr);
        EXPECT_EQ(localScope->lookupLocal("Remote"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresOnlyTheTerminalMember) {
        const auto result = analyze("import pkg.api.value\nlet pkg = 0\nlet api = 0\nexport value\n");

        ASSERT_FALSE(result.hasErrors());
        EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
        EXPECT_EQ(findImportedNode(result, "api"), nullptr);
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope->lookupLocal("count"), nullptr);
        const auto* member = importedModule->getIdentifierByName("value");
        ASSERT_NE(member, nullptr);
        EXPECT_EQ(findImportedNode(result, "value"), member);
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotDeclarePathPrefixesInTheModuleScope) {
        const auto result = analyze("import pkg.api.value\nexport pkg, api\n");

        ASSERT_TRUE(result.hasErrors());
        ASSERT_EQ(result.getErrors().size(), 2);
        EXPECT_EQ(result.getErrors()[0].getMessage(), "Undefined symbol pkg");
        EXPECT_EQ(result.getErrors()[1].getMessage(), "Undefined symbol api");
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresOnlyTheTerminalModule) {
        const auto result = analyze("import pkg.api\nlet pkg = 0\nlet value = 0\nexport api\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "api"), importedModule);
        EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
        EXPECT_EQ(findImportedNode(result, "value"), nullptr);
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope->lookupLocal("count"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, DeclaresImportedSymbolsWithTheirKindsAndOriginalTargets) {
        writeFile(
            "dependency_source/api.vni",
            R"({
    "value": {"category": "let", "type": "int"},
    "run": {"category": "func", "returnType": "void", "parameters": {}, "native": false},
    "Box": {"category": "class", "genericParameters": [], "properties": {}, "methods": {}, "baseClass": null, "implementedInterfaces": [], "final": false},
    "Readable": {"category": "interface", "genericParameters": [], "methods": {}},
    "State": {"category": "enum", "genericParameters": [], "members": {}},
    "Alias": {"category": "typealias", "genericParameters": [], "originalType": "int"},
    "External": {"category": "imported", "source": "extra.tools.enabled"},
    "method": {"category": "method", "returnType": "void", "parameters": {}, "native": false, "static": true, "accessModifier": "public"},
    "Ready": {"category": "enummember", "associatedValues": {}},
    "property": {"category": "property", "type": "int", "static": true, "accessModifier": "public"},
    "parameter": {"category": "parameter", "type": "int"}
})"
        );
        const std::vector<std::pair<std::string, SymbolKind>> identifiers = {
            { "value", SymbolKind::VARIABLE },    { "run", SymbolKind::FUNCTION },      { "Box", SymbolKind::CLASS },           { "Readable", SymbolKind::INTERFACE },
            { "State", SymbolKind::ENUM },        { "Alias", SymbolKind::TYPE_ALIAS },  { "External", SymbolKind::VARIABLE },   { "method", SymbolKind::METHOD },
            { "Ready", SymbolKind::ENUM_MEMBER }, { "property", SymbolKind::PROPERTY }, { "parameter", SymbolKind::PARAMETER },
        };

        for (const auto& [name, kind] : identifiers) {
            for (const bool aliased : { false, true }) {
                const auto source = "import pkg.api." + name + (aliased ? " as selected\n" : "\n");
                SCOPED_TRACE(source);
                const auto result = analyze(source);

                ASSERT_FALSE(result.hasErrors());
                const auto* package = getImportedPackageByName("pkg");
                ASSERT_NE(package, nullptr);
                const auto* importedModule = package->getModuleByName("api");
                ASSERT_NE(importedModule, nullptr);
                const auto* scope = result.getScopeByAstNode(*module);
                ASSERT_NE(scope, nullptr);
                EXPECT_EQ(scope->lookupLocal("pkg"), nullptr);
                EXPECT_EQ(scope->lookupLocal("api"), nullptr);
                const auto* symbol = scope->lookupLocal(aliased ? "selected" : name);
                ASSERT_NE(symbol, nullptr);
                EXPECT_EQ(symbol->getName(), aliased ? "selected" : name);
                EXPECT_EQ(symbol->getKind(), kind);
                EXPECT_EQ(symbol->getOrigin(), SymbolOrigin::IMPORTED);
                EXPECT_EQ(symbol->getLocalNode(), nullptr);
                if (name == "External") {
                    const auto* extraPackage = getImportedPackageByName("extra");
                    ASSERT_NE(extraPackage, nullptr);
                    const auto* toolsModule = extraPackage->getModuleByName("tools");
                    ASSERT_NE(toolsModule, nullptr);
                    EXPECT_EQ(symbol->getImportedNode(), toolsModule->getIdentifierByName("enabled"));
                } else {
                    EXPECT_EQ(symbol->getImportedNode(), importedModule->getIdentifierByName(name));
                }
                for (const auto& [otherName, otherKind] : identifiers) {
                    if (aliased || otherName != name) {
                        EXPECT_EQ(scope->lookupLocal(otherName), nullptr);
                    }
                }
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsRepeatedImportsToTheSameTargets) {
        for (const auto source : {
                 "import pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\nimport pkg.api as library\nimport pkg as package\n",
                 "import pkg as package\nimport pkg.api as library\nimport pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\n",
                 "import pkg.api as library\nimport pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\nimport pkg as package\n",
                 "import pkg.{api.{value, count, value as renamed, self as library}, self as package}\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* package = getImportedPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            const auto* importedModule = package->getModuleByName("api");
            ASSERT_NE(importedModule, nullptr);
            const auto* value = importedModule->getIdentifierByName("value");
            const auto* count = importedModule->getIdentifierByName("count");
            ASSERT_NE(value, nullptr);
            ASSERT_NE(count, nullptr);
            EXPECT_EQ(findImportedNode(result, "value"), value);
            EXPECT_EQ(findImportedNode(result, "renamed"), value);
            EXPECT_EQ(findImportedNode(result, "count"), count);
            EXPECT_EQ(findImportedNode(result, "library"), importedModule);
            EXPECT_EQ(findImportedNode(result, "package"), package);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, AliasesBindToTheirOriginalTargets) {
        const auto result = analyze("import pkg.api.value as renamed\nimport pkg.api as library\nlet value = 0\nlet api = 0\nlet pkg = 0\nexport renamed, library\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "renamed"), importedModule->getIdentifierByName("value"));
        EXPECT_EQ(findImportedNode(result, "library"), importedModule);
        EXPECT_EQ(findImportedNode(result, "value"), nullptr);
        EXPECT_EQ(findImportedNode(result, "api"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, ResolvesReexportedAliasesWhilePreservingDirectRenamedAndWildcardBindings) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"pkg.sub.other.bridge"}})");
        writeFile("dependency_source/sub/other.vni", R"({"bridge":{"category":"imported","source":"extra.tools.enabled"}})");

        for (const auto source : {
                 "import pkg.api.External\nimport pkg.api.External as renamed\nimport extra.tools.enabled\n",
                 "import pkg.api.*\nimport pkg.api.External as renamed\nimport extra.tools.enabled\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* scope = result.getScopeByAstNode(*module);
            ASSERT_NE(scope, nullptr);
            const auto* enabled = findImportedNode(result, "enabled");
            ASSERT_NE(enabled, nullptr);
            EXPECT_EQ(enabled->getName(), "enabled");
            for (const auto name : { "External", "renamed", "enabled" }) {
                const auto* symbol = scope->lookupLocal(name);
                ASSERT_NE(symbol, nullptr);
                EXPECT_EQ(symbol->getName(), name);
                EXPECT_EQ(symbol->getKind(), SymbolKind::VARIABLE);
                EXPECT_EQ(symbol->getOrigin(), SymbolOrigin::IMPORTED);
                EXPECT_EQ(symbol->getImportedNode(), enabled);
            }
            for (const auto name : { "pkg", "api", "bridge", "extra", "tools" }) {
                EXPECT_EQ(scope->lookupLocal(name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RegistersReexportedTypeScopesUnderTheirOriginalPackageAndModule) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"extra.types.Box"},"Alias":{"category":"imported","source":"extra.types.Alias"}})");
        writeFile("another_source/types.vni", R"({
    "Box":{"category":"class","genericParameters":["T"],"properties":{},"methods":{},"baseClass":null,"implementedInterfaces":[],"final":false},
    "Alias":{"category":"typealias","genericParameters":["T"],"originalType":"T"}
})");

        for (const auto source : {
                 "import pkg.api.External as selected\nimport pkg.api.Alias\n",
                 "import pkg.api.*\nimport pkg.api.External as selected\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(std::string(source) + "type ClassResult = selected<int>\ntype AliasResult = Alias<int>\n");

            ASSERT_FALSE(result.hasErrors());
            const auto* package = getImportedPackageByName("extra");
            ASSERT_NE(package, nullptr);
            const auto* typesModule = package->getModuleByName("types");
            ASSERT_NE(typesModule, nullptr);
            const auto* packageScope = result.getScopeByImportedNode(*package);
            const auto* typesScope = result.getScopeByImportedNode(*typesModule);
            ASSERT_NE(packageScope, nullptr);
            ASSERT_NE(typesScope, nullptr);
            EXPECT_EQ(packageScope->findParent(), nullptr);
            EXPECT_EQ(typesScope->findParent(), packageScope);
            const auto* localScope = result.getScopeByAstNode(*module);
            ASSERT_NE(localScope, nullptr);
            for (const auto name : { "selected", "Alias" }) {
                SCOPED_TRACE(name);
                const bool isClass = std::string_view(name) == "selected";
                const auto* symbol = localScope->lookupLocal(name);
                ASSERT_NE(symbol, nullptr);
                EXPECT_EQ(symbol->getKind(), isClass ? SymbolKind::CLASS : SymbolKind::TYPE_ALIAS);
                const auto* node = symbol->getImportedNode();
                ASSERT_EQ(node, typesModule->getIdentifierByName(isClass ? "Box" : "Alias"));
                ASSERT_NE(node, nullptr);
                const auto* scope = result.getScopeByImportedNode(*node);
                ASSERT_NE(scope, nullptr);
                EXPECT_EQ(scope->getKind(), isClass ? ScopeKind::CLASS : ScopeKind::TYPE_ALIAS);
                EXPECT_EQ(scope->getImportedNode(), node);
                EXPECT_EQ(scope->findParent(), typesScope);
                EXPECT_NE(scope->lookupLocal("T"), nullptr);
            }
        }
        EXPECT_EQ(getFullTypeNameByAlias("import pkg.api.External as selected\ntype Result = selected<int>\n"), "extra.types.Box<int>");
    }

    TEST_F(SemanticAnalyzerImportTest, ResolvesNestedImportsAndSelfAliases) {
        const auto result = analyze("import pkg.{api.{self as m, value as v}, sub.other}\nlet pkg = 0\nlet api = 0\nlet sub = 0\nlet value = 0\nexport m, v, other\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        const auto* subPackage = package->getSubPackageByName("sub");
        ASSERT_NE(subPackage, nullptr);
        EXPECT_EQ(findImportedNode(result, "m"), importedModule);
        EXPECT_EQ(findImportedNode(result, "v"), importedModule->getIdentifierByName("value"));
        EXPECT_EQ(findImportedNode(result, "other"), subPackage->getModuleByName("other"));
        EXPECT_EQ(findImportedNode(result, "self"), nullptr);
    }

    TEST_F(SemanticAnalyzerImportTest, ExpandsModuleWildcardsAndCanImportTheModuleItself) {
        for (const auto source : { "import pkg.api.*\nlet api = 0\nexport value, count\n", "import pkg.api.{*, self}\nexport api, value, count\n" }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            EXPECT_NE(findImportedNode(result, "value"), nullptr);
            EXPECT_NE(findImportedNode(result, "count"), nullptr);
            EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
            EXPECT_EQ(findImportedNode(result, "*"), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsPackagesAndPackageSelf) {
        for (const auto source : { "import pkg as library\nexport library\n", "import pkg.{self as library}\nexport library\n" }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* package = getImportedPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            EXPECT_EQ(findImportedNode(result, "library"), package);
            EXPECT_EQ(findImportedNode(result, "api"), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RetainsImportedBindingsAfterTheAnalyzerIsDestroyed) {
        const auto result = analyze("import pkg.api.value\nimport pkg.sub.other.flag\nimport pkg.sub.second.caption\nimport extra.tools.enabled\nexport value, flag, caption, enabled\n");

        ASSERT_FALSE(result.hasErrors());
        for (const auto name : { "value", "flag", "caption", "enabled" }) {
            const auto* binding = findImportedNode(result, name);
            ASSERT_NE(binding, nullptr);
            EXPECT_EQ(binding->getName(), name);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, DoesNotDeclareIndirectImportBindings) {
        writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"External":{"category":"imported","source":"extra.tools.enabled"}})");

        const auto result = analyze("import pkg.api.value\nlet enabled = 0\nlet tools = 0\nlet extra = 0\nexport value, enabled, tools, extra\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "value"), importedModule->getIdentifierByName("value"));
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope->lookupLocal("External"), nullptr);
        for (const auto name : { "enabled", "tools", "extra" }) {
            const auto* symbol = scope->lookupLocal(name);
            ASSERT_NE(symbol, nullptr);
            EXPECT_NE(symbol->getLocalNode(), nullptr);
            EXPECT_EQ(symbol->getImportedNode(), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsReexportedNamesThroughRepeatedAndCyclicModuleDependencies) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"pkg.sub.other.bridge"},"repeated":{"category":"imported","source":"pkg.sub.other.bridge"}})");
        writeFile("dependency_source/sub/other.vni", R"({"bridge":{"category":"imported","source":"extra.tools.enabled"},"back":{"category":"imported","source":"pkg.api.External"}})");

        const auto result = analyze("import pkg.api.External\nexport External\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* extraPackage = getImportedPackageByName("extra");
        ASSERT_NE(extraPackage, nullptr);
        const auto* toolsModule = extraPackage->getModuleByName("tools");
        ASSERT_NE(toolsModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "External"), toolsModule->getIdentifierByName("enabled"));
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        const auto* external = scope->lookupLocal("External");
        ASSERT_NE(external, nullptr);
        EXPECT_EQ(external->getKind(), SymbolKind::VARIABLE);
        for (const auto name : { "repeated", "bridge", "back", "enabled", "other", "tools" }) {
            EXPECT_EQ(scope->lookupLocal(name), nullptr);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, BindsReexportedPackagesAndModules) {
        for (const auto source : { "extra", "extra.tools", "pkg.sub" }) {
            SCOPED_TRACE(source);
            writeFile("dependency_source/api.vni", std::string(R"({"External":{"category":"imported","source":")") + source + R"("}})");

            const auto result = analyze("import pkg.api.External\nexport External\n");

            ASSERT_FALSE(result.hasErrors());
            const ImportedItem* target = nullptr;
            if (std::string_view(source) == "pkg.sub") {
                const auto* package = getImportedPackageByName("pkg");
                ASSERT_NE(package, nullptr);
                const auto* subPackage = package->getSubPackageByName("sub");
                ASSERT_NE(subPackage, nullptr);
                target = subPackage;
            } else {
                const auto* extraPackage = getImportedPackageByName("extra");
                ASSERT_NE(extraPackage, nullptr);
                target = std::string_view(source) == "extra" ? static_cast<const ImportedItem*>(extraPackage) : extraPackage->getModuleByName("tools");
            }
            const auto* scope = result.getScopeByAstNode(*module);
            ASSERT_NE(scope, nullptr);
            const auto* external = scope->lookupLocal("External");
            ASSERT_NE(external, nullptr);
            EXPECT_EQ(external->getImportedNode(), target);
            EXPECT_EQ(external->getKind(), std::string_view(source) == "extra.tools" ? SymbolKind::MODULE : SymbolKind::PACKAGE);
            ASSERT_NE(target, nullptr);
            const auto* targetScope = result.getScopeByImportedNode(*target);
            ASSERT_NE(targetScope, nullptr);
            EXPECT_EQ(targetScope->getKind(), std::string_view(source) == "extra.tools" ? ScopeKind::MODULE : ScopeKind::PACKAGE);
            if (std::string_view(source) == "extra") {
                EXPECT_EQ(targetScope->findParent(), nullptr);
            } else {
                const auto* parentPackage = getImportedPackageByName(std::string_view(source) == "pkg.sub" ? "pkg" : "extra");
                ASSERT_NE(parentPackage, nullptr);
                const auto* parentScope = result.getScopeByImportedNode(*parentPackage);
                ASSERT_NE(parentScope, nullptr);
                EXPECT_EQ(targetScope->findParent(), parentScope);
            }
            for (const auto name : { "extra", "tools", "sub", "other", "second" }) {
                EXPECT_EQ(scope->lookupLocal(name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, PreservesBindingsWhenDirectAndIndirectImportsShareModules) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"extra.tools.enabled"}})");
        writeFile("dependency_source/sub/other.vni", R"({"flag":{"category":"let","type":"bool"},"External":{"category":"imported","source":"extra.tools.enabled"}})");

        for (const auto source : {
                 "import pkg.api.External\nimport extra.tools.enabled\nimport pkg.sub.other.flag\nimport extra.tools as tools\nexport External, enabled, flag, tools\n",
                 "import extra.tools.enabled\nimport pkg.api.External\nimport pkg.sub.other.flag\nimport extra.tools as tools\nexport External, enabled, flag, tools\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* package = getImportedPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            const auto* subPackage = package->getSubPackageByName("sub");
            ASSERT_NE(subPackage, nullptr);
            const auto* otherModule = subPackage->getModuleByName("other");
            ASSERT_NE(otherModule, nullptr);
            EXPECT_EQ(findImportedNode(result, "flag"), otherModule->getIdentifierByName("flag"));
            const auto* extraPackage = getImportedPackageByName("extra");
            ASSERT_NE(extraPackage, nullptr);
            const auto* toolsModule = extraPackage->getModuleByName("tools");
            ASSERT_NE(toolsModule, nullptr);
            EXPECT_EQ(findImportedNode(result, "tools"), toolsModule);
            EXPECT_EQ(findImportedNode(result, "enabled"), toolsModule->getIdentifierByName("enabled"));
            EXPECT_EQ(findImportedNode(result, "External"), findImportedNode(result, "enabled"));
            const auto* scope = result.getScopeByAstNode(*module);
            ASSERT_NE(scope, nullptr);
            const auto* external = scope->lookupLocal("External");
            ASSERT_NE(external, nullptr);
            EXPECT_EQ(external->getKind(), SymbolKind::VARIABLE);
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsMissingAndCyclicAliasTargetsWithoutCommittingPartialBindings) {
        const std::vector<std::pair<std::string_view, std::string_view>> cases = {
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"extra.tools.absent"}})", "Could not find imported alias target" },
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"extra.tools.enabled.member"}})",
              "Could not find imported alias target" },
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"pkg.broken.broken"}})", "Cyclic imported alias" },
            { R"({"valid":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"pkg.broken.link"},"link":{"category":"imported","source":"pkg.broken.broken"}})",
              "Cyclic imported alias" },
        };
        for (const auto& [contents, diagnosticPrefix] : cases) {
            SCOPED_TRACE(contents);
            writeFile("dependency_source/broken.vni", contents);
            for (const auto importSource : { "import pkg.broken.broken\n", "import pkg.broken.{valid, broken}\n", "import pkg.broken.*\n" }) {
                SCOPED_TRACE(importSource);
                std::optional<SemanticResult> result;
                ASSERT_NO_THROW(result.emplace(analyze(std::string("import pkg.api.value as kept\n") + importSource + "let valid = 0\nlet broken = 0\nexport kept\n")));

                ASSERT_TRUE(result->hasErrors());
                for (const auto& error : result->getErrors()) {
                    EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
                    EXPECT_TRUE(error.getMessage().starts_with(diagnosticPrefix)) << error.getMessage();
                }
                const auto* package = getImportedPackageByName("pkg");
                ASSERT_NE(package, nullptr);
                const auto* api = package->getModuleByName("api");
                ASSERT_NE(api, nullptr);
                EXPECT_EQ(findImportedNode(*result, "kept"), api->getIdentifierByName("value"));
                for (const auto name : { "valid", "broken", "link" }) {
                    EXPECT_EQ(findImportedNode(*result, name), nullptr);
                }
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, ReportsInvalidImportsAsSemanticErrors) {
        for (const auto source : {
                 "import absent.api\n",
                 "import pkg.absent\n",
                 "import pkg.api.absent\n",
                 "import pkg.api.value.member\n",
                 "import pkg.api.value.{self}\n",
                 "import pkg.*\n",
                 "import pkg.{api, absent}\n",
                 "import pkg.api.{value, absent}\n",
             }) {
            SCOPED_TRACE(source);
            std::optional<SemanticResult> result;
            ASSERT_NO_THROW(result.emplace(analyze(source, "import pkg\n")));

            ASSERT_TRUE(result->hasErrors());
            for (const auto& error : result->getErrors()) {
                EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
            }
            for (const auto name : { "api", "value" }) {
                EXPECT_EQ(findImportedNode(*result, name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsDuplicateBindingsWithinOneImportAsSemanticErrors) {
        for (const auto source : {
                 "import pkg.api.{value, value}\n",
                 "import pkg.{api.value as same, sub.other.flag as same}\n",
                 "import pkg.api.{*, value}\n",
                 "import pkg.api.{value, *}\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_TRUE(result.hasErrors());
            for (const auto& error : result.getErrors()) {
                EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
                EXPECT_TRUE(error.getMessage().starts_with("Redeclaration of symbol"));
            }
            for (const auto name : { "value", "count", "same" }) {
                EXPECT_EQ(findImportedNode(result, name), nullptr);
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, RejectsRedeclarationsAcrossImportsAndLocalDeclarations) {
        for (const auto source : {
                 "import pkg.api.value\nimport pkg.api.value\n",
                 "import pkg.api.value\nimport pkg.sub.other.flag as value\n",
                 "import pkg.api.value\nlet value = 0\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_TRUE(result.hasErrors());
            EXPECT_NE(findImportedNode(result, "value"), nullptr);
            for (const auto& error : result.getErrors()) {
                EXPECT_EQ(error.getPhase(), DiagnosticPhase::SEMANTIC);
                EXPECT_EQ(error.getMessage(), "Redeclaration of symbol 'value'");
            }
        }
    }

    TEST_F(SemanticAnalyzerImportTest, FailedImportsPreserveEarlierBindingsWithoutDeclaringPartialNames) {
        const auto result = analyze("import pkg.api as kept\nimport pkg.sub.{other as staged, missing}\nlet staged = 0\nexport kept\n", "import pkg\n");

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::SEMANTIC);
        EXPECT_EQ(result.getErrors().front().getMessage(), "Could not find imported package, module or identifier 'missing'");
        const auto* package = getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        EXPECT_EQ(findImportedNode(result, "kept"), package->getModuleByName("api"));
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        const auto* staged = scope->lookupLocal("staged");
        ASSERT_NE(staged, nullptr);
        EXPECT_EQ(staged->getOrigin(), SymbolOrigin::LOCAL);
        EXPECT_NE(staged->getLocalNode(), nullptr);
        EXPECT_EQ(staged->getImportedNode(), nullptr);
    }

} // namespace vnlc
