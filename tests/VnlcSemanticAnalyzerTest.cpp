#include "semantic/VnlcSemanticAnalyzer.hpp"
#include "ast/declaration/VnlcClassDeclarationNode.hpp"
#include "ast/declaration/VnlcEnumDeclarationNode.hpp"
#include "ast/declaration/VnlcInterfaceDeclarationNode.hpp"
#include "ast/declaration/VnlcTypeAliasDeclarationNode.hpp"
#include "ast/statement/VnlcExpressionStatementNode.hpp"
#include "ast/statement/VnlcVariableDeclarationStatementNode.hpp"
#include "config/VnlcConfig.hpp"
#include "lexer/VnlcLexer.hpp"
#include "parser/VnlcParser.hpp"
#include "semantic/VnlcSemanticAnalysisResult.hpp"
#include "semantic/VnlcSemanticContext.hpp"
#include "type/VnlcCustomizedTypeKind.hpp"
#include "type/VnlcCustomizedTypeOrigin.hpp"
#include "type/VnlcTypeExpressionType.hpp"
#include "vni/import/VnlcImportedLet.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

        const auto* customizedType = result.getCustomizedTypeByFullTypeName(fullTypeName);
        ASSERT_NE(customizedType, nullptr);
        EXPECT_EQ(customizedType->getFullTypeName(), fullTypeName);
        EXPECT_EQ(customizedType->getCustomizedKind(), expectedKind);
        EXPECT_EQ(customizedType->getOrigin(), VnlcCustomizedTypeOrigin::LOCAL);
        EXPECT_EQ(customizedType->getLocalNode(), expectedDeclaration);
        EXPECT_EQ(customizedType->getImportedNode(), nullptr);
    }

} // namespace

class VnlcSemanticAnalyzerAccessTest : public testing::Test {
protected:
    const VnlcConfig config = makeConfig("access.vnl");
    std::unique_ptr<VnlcModuleNode> module;
    std::unique_ptr<VnlcClassDeclarationNode> baseClass;
    std::unique_ptr<VnlcClassDeclarationNode> privateShadow;
    std::vector<std::unique_ptr<VnlcModuleNode>> accessModules;
    std::vector<std::unique_ptr<VnlcTypeExpressionType>> typeExpressions;
    std::unique_ptr<VnlcSemanticAnalyzer> analyzer;

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
        const VnlcToken token(VnlcTokenType::IDENTIFIER, "Base", 1, 1, 0);
        std::vector<std::unique_ptr<VnlcDeclarationNode>> members;
        const auto addProperty = [&](std::string_view name, VnlcValueDeclarationType::AccessModifier accessModifier) {
            members.push_back(
                std::make_unique<VnlcValueDeclarationNode>(
                    VnlcValueDeclarationType::Kind::INSTANCE_PROPERTY,
                    VnlcValueDeclarationType::Context::CLASS,
                    accessModifier,
                    std::make_unique<VnlcIdentifierNode>(name, token, token),
                    std::nullopt,
                    std::nullopt,
                    token,
                    token
                )
            );
        };
        addProperty("protectedMember", VnlcValueDeclarationType::AccessModifier::PROTECTED);
        addProperty("privateMember", VnlcValueDeclarationType::AccessModifier::PRIVATE);
        addProperty("publicMember", VnlcValueDeclarationType::AccessModifier::PUBLIC);
        members.push_back(
            std::make_unique<VnlcFunctionDeclarationNode>(
                VnlcFunctionDeclarationType::Kind::NATIVE,
                VnlcFunctionDeclarationType::Context::CLASS,
                VnlcFunctionDeclarationType::AccessModifier::PROTECTED,
                VnlcFunctionDeclarationType::Binding::STATIC,
                std::make_unique<VnlcIdentifierNode>("protectedMethod", token, token),
                std::vector<std::unique_ptr<VnlcValueDeclarationNode>>{},
                std::nullopt,
                std::nullopt,
                token,
                token
            )
        );
        members.push_back(
            std::make_unique<VnlcFunctionDeclarationNode>(
                VnlcFunctionDeclarationType::Kind::NATIVE,
                VnlcFunctionDeclarationType::Context::CLASS,
                VnlcFunctionDeclarationType::AccessModifier::PROTECTED,
                VnlcFunctionDeclarationType::Binding::INSTANCE,
                std::make_unique<VnlcIdentifierNode>("protectedInstanceMethod", token, token),
                std::vector<std::unique_ptr<VnlcValueDeclarationNode>>{},
                std::nullopt,
                std::nullopt,
                token,
                token
            )
        );
        baseClass = std::make_unique<VnlcClassDeclarationNode>(
            false,
            std::make_unique<VnlcIdentifierNode>("Base", token, token),
            std::nullopt,
            std::vector<std::unique_ptr<VnlcTypeNode>>{},
            std::vector<std::unique_ptr<VnlcIdentifierNode>>{},
            std::move(members),
            token,
            token
        );
        members.clear();
        addProperty("protectedMember", VnlcValueDeclarationType::AccessModifier::PRIVATE);
        std::vector<std::unique_ptr<VnlcIdentifierNode>> baseName;
        baseName.push_back(std::make_unique<VnlcIdentifierNode>("Base", token, token));
        privateShadow = std::make_unique<VnlcClassDeclarationNode>(
            false,
            std::make_unique<VnlcIdentifierNode>("PrivateShadow", token, token),
            std::make_unique<VnlcTypeNode>(false, std::move(baseName), std::vector<std::unique_ptr<VnlcTypeNode>>{}, token, token),
            std::vector<std::unique_ptr<VnlcTypeNode>>{},
            std::vector<std::unique_ptr<VnlcIdentifierNode>>{},
            std::move(members),
            token,
            token
        );
        analyzer = std::make_unique<VnlcSemanticAnalyzer>(*module);
        auto& context = analyzer->context;
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::MODULE, nullptr, module.get()));
        std::vector<const VnlcClassDeclarationNode*> classes{ baseClass.get(), privateShadow.get() };
        for (const auto& declaration : module->getTopIdentifierDeclarations()) {
            classes.push_back(&dynamic_cast<const VnlcClassDeclarationNode&>(*declaration));
        }
        for (const auto* classDeclaration : classes) {
            ASSERT_TRUE(
                context.currentScope().declare(VnlcSymbol(VnlcSymbolKind::CLASS, VnlcSymbolAccessModifier::PUBLIC, classDeclaration->getName().getIdentifierString(), classDeclaration))
            );
        }
        accessModules.push_back(parseModule("let privateMember = 0\n", config));
        ASSERT_TRUE(context.currentScope().declare(
            VnlcSymbol(VnlcSymbolKind::VARIABLE, VnlcSymbolAccessModifier::PUBLIC, "privateMember", accessModules.back()->getTopIdentifierDeclarations().front().get())
        ));
        for (const auto* classDeclaration : classes) {
            analyzer->checkClassDeclaration(*classDeclaration, config);
        }
        for (const auto* classDeclaration : classes) {
            if (!classDeclaration->getBaseClass().has_value()) continue;
            const auto* baseType = classDeclaration->getBaseClass().value().get();
            if (const auto* semanticType = findType(baseType->getNameParts().front()->getIdentifierString())) {
                context.mapSemanticType(baseType, semanticType);
            }
        }
        context.popScope();
        ASSERT_TRUE(context.getErrors().empty()) << context.getErrors().front().getMessage();
    }

    const VnlcCustomizedType* findType(std::string_view name) const {
        return analyzer->context.getCustomizedTypeByFullTypeName(std::string(module->getFullName()) + "." + std::string(name));
    }

    const VnlcClassDeclarationNode* findClass(std::string_view name) const {
        if (name == "Base") return baseClass.get();
        if (name == "PrivateShadow") return privateShadow.get();
        for (const auto& declaration : module->getTopIdentifierDeclarations()) {
            const auto* classDeclaration = dynamic_cast<const VnlcClassDeclarationNode*>(declaration.get());
            if (classDeclaration->getName().getIdentifierString() == name) return classDeclaration;
        }
        return nullptr;
    }

    void leaveBaseScopeUnbuilt() {
        auto& context = analyzer->context;
        context = VnlcSemanticContext{};
        const auto fullName = std::string(module->getFullName()) + ".Base";
        context.registerCustomizedType(fullName, std::make_unique<VnlcCustomizedType>(VnlcCustomizedTypeKind::CLASS, fullName, baseClass.get()));
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::MODULE, nullptr, module.get()));
        context.popScope();
    }

    void prepareDerivedScopeWithoutBaseScope() {
        leaveBaseScopeUnbuilt();
        auto& context = analyzer->context;
        const auto& derivedClass = dynamic_cast<const VnlcClassDeclarationNode&>(*module->getTopIdentifierDeclarations().front());
        const auto fullName = std::string(module->getFullName()) + ".Derived";
        context.registerCustomizedType(fullName, std::make_unique<VnlcCustomizedType>(VnlcCustomizedTypeKind::CLASS, fullName, &derivedClass));
        context.mapSemanticType(derivedClass.getBaseClass().value().get(), findType("Base"));
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::CLASS, context.getScopeByAstNode(module.get()), &derivedClass));
        context.popScope();
    }

    void prepareUnregisteredDerivedScope() {
        leaveBaseScopeUnbuilt();
        auto& context = analyzer->context;
        const auto* derivedClass = findClass("Derived");
        context.mapSemanticType(derivedClass->getBaseClass().value().get(), findType("Base"));
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::CLASS, context.getScopeByAstNode(module.get()), derivedClass));
    }

    bool canAccess(std::string_view receiver, std::string_view accessor, std::string_view member, bool typeExpression = false, std::string_view object = "object") {
        accessModules.push_back(parseModule("func inspect() {\n" + std::string(object) + "." + std::string(member) + "\n}\n", config));
        const auto& function = dynamic_cast<const VnlcFunctionDeclarationNode&>(*accessModules.back()->getTopIdentifierDeclarations().front());
        const auto& statement = dynamic_cast<const VnlcExpressionStatementNode&>(*function.getBody().value()->getStatements().front());
        const auto& access = dynamic_cast<const VnlcMemberAccessExpressionNode&>(statement.getExpression());
        auto& context = analyzer->context;
        const auto* parent = context.getScopeByAstNode(accessor.empty() ? static_cast<const VnlcAstNode*>(module.get()) : findType(accessor)->getLocalNode());
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::FUNCTION, parent, &function));
        const VnlcSemanticType* receiverType = findType(receiver);
        if (typeExpression) {
            typeExpressions.push_back(std::make_unique<VnlcTypeExpressionType>(receiverType));
            receiverType = typeExpressions.back().get();
        }
        context.mapInferredExpressionType(&access.getObject(), receiverType);
        const bool allowed = analyzer->checkAccessModifier(access);
        context.popScope();
        return allowed;
    }

    bool canAccessIdentifier(std::string_view accessor, std::string_view identifier, std::optional<VnlcSymbolKind> localSymbolKind = std::nullopt, bool nestedBlock = false) {
        const auto parameter = localSymbolKind == VnlcSymbolKind::PARAMETER ? std::string(identifier) + ": int" : "";
        const auto variable = localSymbolKind == VnlcSymbolKind::VARIABLE ? "let " + std::string(identifier) + " = 0\n" : "";
        const auto body = variable + std::string(identifier) + "\n";
        accessModules.push_back(parseModule("func inspect(" + parameter + ") {\n" + (nestedBlock ? "{\n" + body + "}\n" : body) + "}\n", config));
        const auto& function = dynamic_cast<const VnlcFunctionDeclarationNode&>(*accessModules.back()->getTopIdentifierDeclarations().front());
        const auto& bodyBlock = *function.getBody().value();
        const auto& block = nestedBlock ? dynamic_cast<const VnlcBlockStatementNode&>(*bodyBlock.getStatements().front()) : bodyBlock;
        const auto& statement = dynamic_cast<const VnlcExpressionStatementNode&>(*block.getStatements().back());
        const auto& expression = dynamic_cast<const VnlcIdentifierExpressionNode&>(statement.getExpression());
        auto& context = analyzer->context;
        const auto* parent = context.getScopeByAstNode(accessor.empty() ? static_cast<const VnlcAstNode*>(module.get()) : findClass(accessor));
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::FUNCTION, parent, &function));
        if (localSymbolKind == VnlcSymbolKind::PARAMETER) {
            EXPECT_TRUE(context.currentScope().declare(VnlcSymbol(VnlcSymbolKind::PARAMETER, VnlcSymbolAccessModifier::PUBLIC, identifier, function.getParameters().front().get())));
        }
        if (nestedBlock) context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::BLOCK, &context.currentScope(), &block));
        if (localSymbolKind == VnlcSymbolKind::VARIABLE) {
            const auto& declaration = dynamic_cast<const VnlcVariableDeclarationStatementNode&>(*block.getStatements().front()).getVariableDeclaration();
            EXPECT_TRUE(context.currentScope().declare(VnlcSymbol(VnlcSymbolKind::VARIABLE, VnlcSymbolAccessModifier::PUBLIC, identifier, &declaration)));
        }
        const bool allowed = analyzer->checkAccessModifier(expression);
        if (nestedBlock) context.popScope();
        context.popScope();
        return allowed;
    }
};

TEST_F(VnlcSemanticAnalyzerAccessTest, AllowsImplicitThisAccessWithinTheDeclaringClass) {
    for (const auto member : { "publicMember", "protectedMember", "privateMember", "protectedInstanceMethod", "protectedMethod" }) {
        SCOPED_TRACE(member);
        EXPECT_TRUE(canAccessIdentifier("Base", member));
    }
}

TEST_F(VnlcSemanticAnalyzerAccessTest, AllowsInheritedProtectedMembersThroughImplicitThis) {
    for (const auto accessor : { "Derived", "Descendant" }) {
        SCOPED_TRACE(accessor);
        for (const auto member : { "publicMember", "protectedMember", "protectedInstanceMethod", "protectedMethod" }) {
            SCOPED_TRACE(member);
            EXPECT_TRUE(canAccessIdentifier(accessor, member));
        }
    }
}

TEST_F(VnlcSemanticAnalyzerAccessTest, DeniesInheritedPrivateMembersBeforeLookingUpModuleVariables) {
    EXPECT_FALSE(canAccessIdentifier("Derived", "privateMember"));
    EXPECT_FALSE(canAccessIdentifier("Descendant", "privateMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, LocalVariablesAndParametersHideInheritedPrivateMembers) {
    for (const auto localSymbolKind : { VnlcSymbolKind::VARIABLE, VnlcSymbolKind::PARAMETER }) {
        for (const bool nestedBlock : { false, true }) {
            SCOPED_TRACE(nestedBlock);
            EXPECT_TRUE(canAccessIdentifier("Derived", "privateMember", localSymbolKind, nestedBlock));
        }
    }
}

TEST_F(VnlcSemanticAnalyzerAccessTest, ResolvesClassSymbolsBeforeInheritedMembersForImplicitThis) {
    EXPECT_TRUE(canAccessIdentifier("PublicShadow", "privateMember"));
    EXPECT_TRUE(canAccessIdentifier("PrivateShadow", "protectedMember"));
    EXPECT_TRUE(canAccessIdentifier("GenericPrivateShadow", "privateMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, DefersIdentifierResolutionErrorsToOtherChecks) {
    EXPECT_TRUE(canAccessIdentifier("", "privateMember"));
    EXPECT_TRUE(canAccessIdentifier("", "protectedMember"));
    EXPECT_TRUE(canAccessIdentifier("Derived", "missingMember"));
    EXPECT_TRUE(canAccessIdentifier("MissingBase", "missingMember"));
    EXPECT_TRUE(canAccessIdentifier("CycleA", "missingMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, ChecksImplicitThisBeforeTheCurrentClassTypeAndBaseScopeAreRegistered) {
    prepareUnregisteredDerivedScope();
    EXPECT_TRUE(canAccessIdentifier("Derived", "protectedMember"));
    EXPECT_TRUE(canAccessIdentifier("Derived", "protectedInstanceMethod"));
    EXPECT_TRUE(canAccessIdentifier("Derived", "protectedMethod"));
    EXPECT_FALSE(canAccessIdentifier("Derived", "privateMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, AllowsProtectedMembersWithinDeclaringAndDerivedClasses) {
    for (const auto accessor : { "Base", "Derived", "Descendant" }) {
        SCOPED_TRACE(accessor);
        EXPECT_TRUE(canAccess(accessor, accessor, "protectedMember"));
        EXPECT_TRUE(canAccess(accessor, accessor, "protectedInstanceMethod"));
        EXPECT_TRUE(canAccess("Base", accessor, "protectedMethod", true));
    }
}

TEST_F(VnlcSemanticAnalyzerAccessTest, DeniesProtectedInstanceMembersThroughBaseOrSiblingInstances) {
    for (const auto member : { "protectedMember", "protectedInstanceMethod" }) {
        SCOPED_TRACE(member);
        EXPECT_FALSE(canAccess("Base", "Derived", member));
        EXPECT_FALSE(canAccess("Base", "Descendant", member));
        EXPECT_FALSE(canAccess("Derived", "Descendant", member));
        EXPECT_FALSE(canAccess("GenericShadow", "Derived", member));
        EXPECT_TRUE(canAccess("Descendant", "Derived", member));
    }
}

TEST_F(VnlcSemanticAnalyzerAccessTest, AllowsProtectedInstanceMembersThroughThisAndSuper) {
    for (const auto member : { "protectedMember", "protectedInstanceMethod" }) {
        SCOPED_TRACE(member);
        EXPECT_TRUE(canAccess("Derived", "Derived", member, false, "this"));
        EXPECT_TRUE(canAccess("Descendant", "Descendant", member, false, "this"));
        EXPECT_TRUE(canAccess("Base", "Derived", member, false, "super"));
        EXPECT_TRUE(canAccess("Derived", "Descendant", member, false, "super"));
    }
}

TEST_F(VnlcSemanticAnalyzerAccessTest, AllowsProtectedStaticMethodsThroughBaseAndSiblingInstances) {
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

TEST_F(VnlcSemanticAnalyzerAccessTest, DeniesProtectedMembersOutsideTheirHierarchy) {
    for (const auto accessor : { "Unrelated", "" }) {
        SCOPED_TRACE(accessor);
        EXPECT_FALSE(canAccess("Base", accessor, "protectedMember"));
        EXPECT_FALSE(canAccess("Base", accessor, "protectedMethod", true));
        EXPECT_FALSE(canAccess("Base", accessor, "protectedMember", false, "super"));
        EXPECT_FALSE(canAccess("Base", accessor, "protectedInstanceMethod", false, "super"));
    }
}

TEST_F(VnlcSemanticAnalyzerAccessTest, ChecksInheritedMembersAgainstTheirDeclaringClass) {
    EXPECT_TRUE(canAccess("Descendant", "Base", "protectedMember"));
    EXPECT_TRUE(canAccess("Descendant", "Derived", "protectedMember"));
    EXPECT_TRUE(canAccess("Descendant", "Descendant", "protectedMethod", true));
    EXPECT_FALSE(canAccess("Descendant", "Unrelated", "protectedMember"));
    EXPECT_FALSE(canAccess("Descendant", "", "protectedMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, PreservesPublicAndPrivateAccessRules) {
    EXPECT_TRUE(canAccess("Base", "", "publicMember"));
    EXPECT_TRUE(canAccess("Descendant", "", "publicMember"));
    EXPECT_TRUE(canAccess("Base", "Base", "privateMember"));
    EXPECT_TRUE(canAccess("Descendant", "Base", "privateMember"));
    EXPECT_FALSE(canAccess("Base", "Derived", "privateMember"));
    EXPECT_FALSE(canAccess("Base", "", "privateMember"));
    EXPECT_FALSE(canAccess("Descendant", "Descendant", "privateMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, DoesNotGrantProtectedAccessForMissingOrCyclicBaseTypes) {
    EXPECT_FALSE(canAccess("Base", "MissingBase", "protectedMember"));
    EXPECT_FALSE(canAccess("Base", "CycleA", "protectedMember"));
    EXPECT_TRUE(canAccess("CycleA", "Unrelated", "missingMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, ChecksDeclarationsBeforeTheirScopesAreBuilt) {
    leaveBaseScopeUnbuilt();
    EXPECT_FALSE(canAccess("Base", "", "protectedMember"));
    EXPECT_FALSE(canAccess("Base", "", "protectedMethod", true));
    EXPECT_FALSE(canAccess("Base", "", "privateMember"));
    EXPECT_TRUE(canAccess("Base", "", "publicMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, ChecksProtectedReceiversBeforeTheBaseScopeIsBuilt) {
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

TEST_F(VnlcSemanticAnalyzerAccessTest, UsesAccessModifierOfTheNearestMemberDeclaration) {
    EXPECT_TRUE(canAccess("PublicShadow", "", "protectedMember"));
    EXPECT_TRUE(canAccess("PrivateShadow", "PrivateShadow", "protectedMember"));
    EXPECT_FALSE(canAccess("PrivateShadow", "", "protectedMember"));
    EXPECT_FALSE(canAccess("PrivateShadow", "Base", "protectedMember"));
}

TEST_F(VnlcSemanticAnalyzerAccessTest, GenericParametersDoNotHideInheritedProtectedMembers) {
    EXPECT_FALSE(canAccess("GenericShadow", "", "protectedMember"));
    EXPECT_FALSE(canAccess("GenericShadow", "Unrelated", "protectedMember"));
    EXPECT_TRUE(canAccess("GenericShadow", "GenericShadow", "protectedMember"));
}

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
    EXPECT_EQ(result.getCustomizedTypeByFullTypeName("semantic_test_package.models.registration_errors.InvalidClass"), nullptr);

    const auto& declarations = module->getTopIdentifierDeclarations();
    ASSERT_EQ(declarations.size(), 2);
    const auto* validInterfaceDeclaration = dynamic_cast<const VnlcInterfaceDeclarationNode*>(declarations[1].get());
    ASSERT_NE(validInterfaceDeclaration, nullptr);
    expectRegisteredCustomizedType(result, "semantic_test_package.models.registration_errors.ValidInterface", VnlcCustomizedTypeKind::INTERFACE, validInterfaceDeclaration);
}

TEST(VnlcSemanticContextTest, RetainsPoppedScopesAndTheirParents) {
    const auto config = makeConfig("scopes.vnl");
    const auto module = parseModule("class Sample {}", config);
    const auto* classDeclaration = module->getTopIdentifierDeclarations().front().get();
    VnlcSemanticContext context;

    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::MODULE, nullptr, module.get()));
    const auto* moduleScope = &context.currentScope();
    ASSERT_TRUE(context.currentScope().declare(VnlcSymbol(VnlcSymbolKind::CLASS, VnlcSymbolAccessModifier::PUBLIC, "Sample", classDeclaration)));
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::CLASS, moduleScope, classDeclaration));
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

TEST(VnlcSemanticContextTest, RetainsLocalAndImportedScopesSeparately) {
    const auto config = makeConfig("scopes.vnl");
    const auto module = parseModule("", config);
    const VnlcImportedModule importedModule("api", {});
    const VnlcImportedModule anotherModule("other", {});
    VnlcSemanticContext context;

    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::MODULE, nullptr, module.get()));
    const auto* localScope = &context.currentScope();
    EXPECT_EQ(localScope->getOrigin(), VnlcScopeOrigin::LOCAL);
    EXPECT_EQ(localScope->getLocalNode(), module.get());
    EXPECT_EQ(localScope->getImportedNode(), nullptr);

    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::MODULE, nullptr, &importedModule));
    const auto* importedScope = &context.currentScope();
    EXPECT_EQ(importedScope->getOrigin(), VnlcScopeOrigin::IMPORTED);
    EXPECT_EQ(importedScope->getLocalNode(), nullptr);
    EXPECT_EQ(importedScope->getImportedNode(), &importedModule);
    EXPECT_EQ(context.getScopeByAstNode(nullptr), nullptr);
    EXPECT_EQ(context.getScopeByImportedNode(nullptr), nullptr);
    EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
    EXPECT_EQ(context.getScopeByAstNode(module.get()), localScope);
    EXPECT_EQ(&context.getOrCreateImportedScope(VnlcScopeKind::MODULE, nullptr, importedModule), importedScope);

    context.popScope();
    EXPECT_EQ(context.getScopeByImportedNode(&importedModule), importedScope);
    EXPECT_EQ(&context.getOrCreateImportedScope(VnlcScopeKind::MODULE, nullptr, importedModule), importedScope);
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::MODULE, nullptr, &anotherModule));
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

class VnlcSemanticAnalyzerImportTest : public testing::Test {
protected:
    std::filesystem::path testDirectory;
    VnlcConfig config = makeConfig("imports.vnl");
    std::unique_ptr<VnlcModuleNode> module;

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

    VnlcSemanticAnalysisResult analyze(std::string_view source) {
        module = parseModule(source, config);
        VnlcSemanticAnalyzer analyzer(*module);
        return analyzer.analyze(config);
    }

    const VnlcImportedItem* findImportedNode(const VnlcSemanticAnalysisResult& result, std::string_view name) const {
        const auto* scope = result.getScopeByAstNode(*module);
        EXPECT_NE(scope, nullptr);
        if (scope == nullptr) return nullptr;
        const auto* symbol = scope->lookupLocal(name);
        return symbol != nullptr ? symbol->getImportedNode() : nullptr;
    }
};

TEST_F(VnlcSemanticAnalyzerImportTest, BuildsImportedPackageModuleAndMemberScopes) {
    writeScopedModule();
    const auto result = analyze("import pkg as library\nlet localOnly = 0\nexport library\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* localScope = result.getScopeByAstNode(*module);
    ASSERT_NE(localScope, nullptr);
    EXPECT_EQ(localScope->getOrigin(), VnlcScopeOrigin::LOCAL);
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* packageScope = result.getScopeByImportedNode(*package);
    ASSERT_NE(packageScope, nullptr);
    EXPECT_EQ(packageScope->getKind(), VnlcScopeKind::PACKAGE);
    EXPECT_EQ(packageScope->getOrigin(), VnlcScopeOrigin::IMPORTED);
    EXPECT_EQ(packageScope->getImportedNode(), package);
    EXPECT_EQ(packageScope->getLocalNode(), nullptr);
    EXPECT_EQ(packageScope->findParent(), nullptr);
    EXPECT_EQ(findImportedNode(result, "library"), package);

    const auto* api = package->getModuleByName("api");
    ASSERT_NE(api, nullptr);
    const auto* apiScope = result.getScopeByImportedNode(*api);
    ASSERT_NE(apiScope, nullptr);
    EXPECT_EQ(apiScope->getKind(), VnlcScopeKind::MODULE);
    EXPECT_EQ(apiScope->findParent(), packageScope);
    const auto* apiSymbol = packageScope->lookupLocal("api");
    ASSERT_NE(apiSymbol, nullptr);
    EXPECT_EQ(apiSymbol->getKind(), VnlcSymbolKind::MODULE);
    EXPECT_EQ(apiSymbol->getImportedNode(), api);

    const auto expectChildScope = [&](const VnlcScope& parent, std::string_view name, VnlcScopeKind kind) -> const VnlcScope* {
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
            EXPECT_EQ(scope->getOrigin(), VnlcScopeOrigin::IMPORTED);
            EXPECT_EQ(scope->getImportedNode(), node);
            EXPECT_EQ(scope->getLocalNode(), nullptr);
            EXPECT_EQ(scope->findParent(), &parent);
        }
        return scope;
    };
    const auto* subScope = expectChildScope(*packageScope, "sub", VnlcScopeKind::PACKAGE);
    ASSERT_NE(subScope, nullptr);
    EXPECT_NE(expectChildScope(*subScope, "other", VnlcScopeKind::MODULE), nullptr);

    const auto* boxScope = expectChildScope(*apiScope, "Box", VnlcScopeKind::CLASS);
    ASSERT_NE(boxScope, nullptr);
    const auto* visible = boxScope->lookupLocal("visible");
    const auto* secret = boxScope->lookupLocal("secret");
    const auto* method = boxScope->lookupLocal("apply");
    ASSERT_NE(visible, nullptr);
    ASSERT_NE(secret, nullptr);
    ASSERT_NE(method, nullptr);
    EXPECT_EQ(visible->getKind(), VnlcSymbolKind::PROPERTY);
    EXPECT_EQ(visible->getAccessModifier(), VnlcSymbolAccessModifier::PUBLIC);
    EXPECT_EQ(secret->getAccessModifier(), VnlcSymbolAccessModifier::PRIVATE);
    EXPECT_EQ(method->getKind(), VnlcSymbolKind::METHOD);
    EXPECT_EQ(method->getAccessModifier(), VnlcSymbolAccessModifier::PROTECTED);
    EXPECT_EQ(result.getScopeByImportedNode(*visible->getImportedNode()), nullptr);
    const auto* methodScope = expectChildScope(*boxScope, "apply", VnlcScopeKind::FUNCTION);
    ASSERT_NE(methodScope, nullptr);
    const auto* input = methodScope->lookupLocal("input");
    ASSERT_NE(input, nullptr);
    EXPECT_EQ(input->getKind(), VnlcSymbolKind::PARAMETER);
    EXPECT_EQ(input->getOrigin(), VnlcSymbolOrigin::IMPORTED);
    EXPECT_EQ(result.getScopeByImportedNode(*input->getImportedNode()), nullptr);
    const auto* generic = boxScope->lookupLocal("T");
    ASSERT_NE(generic, nullptr);
    EXPECT_EQ(generic->getKind(), VnlcSymbolKind::GENERIC_PARAMETER);
    EXPECT_EQ(generic->getImportedNode(), boxScope->getImportedNode());
    EXPECT_EQ(methodScope->lookup("T"), generic);
    EXPECT_EQ(methodScope->lookup("localOnly"), nullptr);

    const auto* interfaceScope = expectChildScope(*apiScope, "Readable", VnlcScopeKind::INTERFACE);
    ASSERT_NE(interfaceScope, nullptr);
    const auto* readScope = expectChildScope(*interfaceScope, "read", VnlcScopeKind::FUNCTION);
    ASSERT_NE(readScope, nullptr);
    EXPECT_NE(readScope->lookupLocal("input"), nullptr);
    EXPECT_NE(interfaceScope->lookupLocal("R"), nullptr);
    EXPECT_EQ(readScope->lookup("R"), interfaceScope->lookupLocal("R"));

    const auto* enumScope = expectChildScope(*apiScope, "State", VnlcScopeKind::ENUM);
    ASSERT_NE(enumScope, nullptr);
    const auto* memberScope = expectChildScope(*enumScope, "Ready", VnlcScopeKind::ENUM_MEMBER);
    ASSERT_NE(memberScope, nullptr);
    const auto* payload = memberScope->lookupLocal("payload");
    ASSERT_NE(payload, nullptr);
    EXPECT_EQ(payload->getKind(), VnlcSymbolKind::PROPERTY);
    EXPECT_EQ(payload->getOrigin(), VnlcSymbolOrigin::IMPORTED);
    EXPECT_EQ(result.getScopeByImportedNode(*payload->getImportedNode()), nullptr);
    EXPECT_NE(enumScope->lookupLocal("E"), nullptr);
    EXPECT_EQ(memberScope->lookup("E"), enumScope->lookupLocal("E"));

    const auto* aliasScope = expectChildScope(*apiScope, "Alias", VnlcScopeKind::TYPE_ALIAS);
    ASSERT_NE(aliasScope, nullptr);
    EXPECT_NE(aliasScope->lookupLocal("A"), nullptr);
    const auto* functionScope = expectChildScope(*apiScope, "run", VnlcScopeKind::FUNCTION);
    ASSERT_NE(functionScope, nullptr);
    EXPECT_NE(functionScope->lookupLocal("amount"), nullptr);
    const auto* external = apiScope->lookupLocal("External");
    ASSERT_NE(external, nullptr);
    EXPECT_EQ(external->getKind(), VnlcSymbolKind::IMPORT_ALIAS);
    EXPECT_EQ(result.getScopeByImportedNode(*external->getImportedNode()), nullptr);
    const auto* extraPackage = result.getImportedPackageByName("extra");
    ASSERT_NE(extraPackage, nullptr);
    EXPECT_EQ(result.getScopeByImportedNode(*extraPackage), nullptr);
    for (const auto name : { "api", "Box", "T", "apply", "input", "Ready", "payload", "extra", "External" }) {
        EXPECT_EQ(localScope->lookupLocal(name), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, CreatesScopesForDirectAndWildcardImportsOfScopedIdentifiers) {
    writeScopedModule();
    const std::vector<std::pair<std::string, VnlcScopeKind>> scopedIdentifiers = {
        { "run", VnlcScopeKind::FUNCTION },     { "Box", VnlcScopeKind::CLASS },       { "Readable", VnlcScopeKind::INTERFACE }, { "State", VnlcScopeKind::ENUM },
        { "Alias", VnlcScopeKind::TYPE_ALIAS }, { "method", VnlcScopeKind::FUNCTION }, { "Ready", VnlcScopeKind::ENUM_MEMBER },
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

TEST_F(VnlcSemanticAnalyzerImportTest, DoesNotCreateScopesForImportedValuesOrUnresolvedAliases) {
    writeScopedModule();
    for (const auto name : { "value", "property", "parameter", "External" }) {
        SCOPED_TRACE(name);
        const auto result = analyze(std::string("import pkg.api.") + name + "\n");
        ASSERT_FALSE(result.hasErrors());
        const auto* node = findImportedNode(result, name);
        ASSERT_NE(node, nullptr);
        EXPECT_EQ(result.getScopeByImportedNode(*node), nullptr);
        const auto* package = result.getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        EXPECT_EQ(result.getScopeByImportedNode(*package), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, ReusesImportedScopesAcrossAliasesAndImportOrders) {
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

TEST_F(VnlcSemanticAnalyzerImportTest, UpdatesExistingImportedPackageScopesWhenNewModulesAreLoaded) {
    const auto result = analyze("import pkg.api as api\nimport pkg.sub.other.flag\n");
    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
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

TEST_F(VnlcSemanticAnalyzerImportTest, UpdatesExistingScopesWhenIndirectImportsExtendAnotherPackage) {
    writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"extra.types.Remote"}})");
    writeFile("another_source/types.vni", R"({"Remote":{"category":"typealias","genericParameters":["T"],"originalType":"T"}})");
    const auto result = analyze("import extra.tools as tools\nimport pkg.api.value\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("extra");
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

TEST_F(VnlcSemanticAnalyzerImportTest, DeclaresOnlyTheTerminalMemberAndRetainsItsModule) {
    const auto result = analyze("import pkg.api.value\nlet pkg = 0\nlet api = 0\nexport value\n");

    ASSERT_FALSE(result.hasErrors());
    EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
    EXPECT_EQ(findImportedNode(result, "api"), nullptr);
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    EXPECT_EQ(package->getName(), "pkg");
    const auto* importedModule = package->getModuleByName("api");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(importedModule->getName(), "api");
    EXPECT_EQ(importedModule->getIdentifiers().size(), 2);
    EXPECT_NE(importedModule->getIdentifierByName("count"), nullptr);
    const auto* scope = result.getScopeByAstNode(*module);
    ASSERT_NE(scope, nullptr);
    EXPECT_EQ(scope->lookupLocal("count"), nullptr);
    const auto* member = importedModule->getIdentifierByName("value");
    ASSERT_NE(member, nullptr);
    EXPECT_EQ(findImportedNode(result, "value"), member);
    const auto* value = dynamic_cast<const VnlcImportedLet*>(member);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->getType(), "int");
}

TEST_F(VnlcSemanticAnalyzerImportTest, DoesNotDeclarePathPrefixesInTheModuleScope) {
    const auto result = analyze("import pkg.api.value\nexport pkg, api\n");

    ASSERT_TRUE(result.hasErrors());
    ASSERT_EQ(result.getErrors().size(), 2);
    EXPECT_EQ(result.getErrors()[0].getMessage(), "Undefined symbol pkg");
    EXPECT_EQ(result.getErrors()[1].getMessage(), "Undefined symbol api");
}

TEST_F(VnlcSemanticAnalyzerImportTest, DeclaresOnlyTheTerminalModule) {
    const auto result = analyze("import pkg.api\nlet pkg = 0\nlet value = 0\nexport api\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* importedModule = package->getModuleByName("api");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(importedModule->getIdentifiers().size(), 2);
    EXPECT_NE(importedModule->getIdentifierByName("value"), nullptr);
    EXPECT_NE(importedModule->getIdentifierByName("count"), nullptr);
    EXPECT_EQ(findImportedNode(result, "api"), importedModule);
    EXPECT_EQ(findImportedNode(result, "pkg"), nullptr);
    EXPECT_EQ(findImportedNode(result, "value"), nullptr);
    const auto* scope = result.getScopeByAstNode(*module);
    ASSERT_NE(scope, nullptr);
    EXPECT_EQ(scope->lookupLocal("count"), nullptr);
}

TEST_F(VnlcSemanticAnalyzerImportTest, RegistersEveryIdentifierKindAgainstTheCompleteModuleTree) {
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
    const std::vector<std::pair<std::string, VnlcSymbolKind>> identifiers = {
        { "value", VnlcSymbolKind::VARIABLE },        { "run", VnlcSymbolKind::FUNCTION },        { "Box", VnlcSymbolKind::CLASS },
        { "Readable", VnlcSymbolKind::INTERFACE },    { "State", VnlcSymbolKind::ENUM },          { "Alias", VnlcSymbolKind::TYPE_ALIAS },
        { "External", VnlcSymbolKind::IMPORT_ALIAS }, { "method", VnlcSymbolKind::METHOD },       { "Ready", VnlcSymbolKind::ENUM_MEMBER },
        { "property", VnlcSymbolKind::PROPERTY },     { "parameter", VnlcSymbolKind::PARAMETER },
    };

    for (const auto& [name, kind] : identifiers) {
        for (const bool aliased : { false, true }) {
            const auto source = "import pkg.api." + name + (aliased ? " as selected\n" : "\n");
            SCOPED_TRACE(source);
            const auto result = analyze(source);

            ASSERT_FALSE(result.hasErrors());
            const auto* package = result.getImportedPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            const auto* importedModule = package->getModuleByName("api");
            ASSERT_NE(importedModule, nullptr);
            EXPECT_EQ(importedModule->getIdentifiers().size(), identifiers.size());
            const auto* scope = result.getScopeByAstNode(*module);
            ASSERT_NE(scope, nullptr);
            EXPECT_EQ(scope->lookupLocal("pkg"), nullptr);
            EXPECT_EQ(scope->lookupLocal("api"), nullptr);
            const auto* symbol = scope->lookupLocal(aliased ? "selected" : name);
            ASSERT_NE(symbol, nullptr);
            EXPECT_EQ(symbol->getName(), aliased ? "selected" : name);
            EXPECT_EQ(symbol->getKind(), kind);
            EXPECT_EQ(symbol->getOrigin(), VnlcSymbolOrigin::IMPORTED);
            EXPECT_EQ(symbol->getLocalNode(), nullptr);
            EXPECT_EQ(symbol->getImportedNode(), importedModule->getIdentifierByName(name));
            for (const auto& [otherName, otherKind] : identifiers) {
                EXPECT_NE(importedModule->getIdentifierByName(otherName), nullptr);
                if (aliased || otherName != name) {
                    EXPECT_EQ(scope->lookupLocal(otherName), nullptr);
                }
            }
        }
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, ReusesIdentifierNodesAcrossModuleAndPackageImports) {
    for (const auto source : {
             "import pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\nimport pkg.api as library\nimport pkg as package\n",
             "import pkg as package\nimport pkg.api as library\nimport pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\n",
             "import pkg.api as library\nimport pkg.api.value\nimport pkg.api.count\nimport pkg.api.value as renamed\nimport pkg as package\n",
             "import pkg.{api.{value, count, value as renamed, self as library}, self as package}\n",
         }) {
        SCOPED_TRACE(source);
        const auto result = analyze(source);

        ASSERT_FALSE(result.hasErrors());
        const auto* package = result.getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(importedModule->getIdentifiers().size(), 2);
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

TEST_F(VnlcSemanticAnalyzerImportTest, AliasesBindToTheirOriginalTargets) {
    const auto result = analyze("import pkg.api.value as renamed\nimport pkg.api as library\nlet value = 0\nlet api = 0\nlet pkg = 0\nexport renamed, library\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* importedModule = package->getModuleByName("api");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(findImportedNode(result, "renamed"), importedModule->getIdentifierByName("value"));
    EXPECT_EQ(findImportedNode(result, "library"), importedModule);
    EXPECT_EQ(findImportedNode(result, "value"), nullptr);
    EXPECT_EQ(findImportedNode(result, "api"), nullptr);
}

TEST_F(VnlcSemanticAnalyzerImportTest, ResolvesNestedImportsAndSelfAliases) {
    const auto result = analyze("import pkg.{api.{self as m, value as v}, sub.other}\nlet pkg = 0\nlet api = 0\nlet sub = 0\nlet value = 0\nexport m, v, other\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
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

TEST_F(VnlcSemanticAnalyzerImportTest, ExpandsModuleWildcardsAndCanImportTheModuleItself) {
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

TEST_F(VnlcSemanticAnalyzerImportTest, ImportsCompletePackagesAndPackageSelf) {
    for (const auto source : { "import pkg as library\nexport library\n", "import pkg.{self as library}\nexport library\n" }) {
        SCOPED_TRACE(source);
        const auto result = analyze(source);

        ASSERT_FALSE(result.hasErrors());
        const auto* package = result.getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        EXPECT_EQ(findImportedNode(result, "library"), package);
        EXPECT_NE(package->getModuleByName("api"), nullptr);
        const auto* subPackage = package->getSubPackageByName("sub");
        ASSERT_NE(subPackage, nullptr);
        EXPECT_NE(subPackage->getModuleByName("other"), nullptr);
        EXPECT_NE(subPackage->getModuleByName("second"), nullptr);
        EXPECT_EQ(findImportedNode(result, "api"), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, RetainsMultipleModulesAndPackagesAfterTheAnalyzerIsDestroyed) {
    const auto result = analyze("import pkg.api.value\nimport pkg.sub.other.flag\nimport pkg.sub.second.caption\nimport extra.tools.enabled\nexport value, flag, caption, enabled\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    EXPECT_NE(package->getModuleByName("api"), nullptr);
    const auto* subPackage = package->getSubPackageByName("sub");
    ASSERT_NE(subPackage, nullptr);
    EXPECT_NE(subPackage->getModuleByName("other"), nullptr);
    EXPECT_NE(subPackage->getModuleByName("second"), nullptr);
    const auto* extraPackage = result.getImportedPackageByName("extra");
    ASSERT_NE(extraPackage, nullptr);
    EXPECT_NE(extraPackage->getModuleByName("tools"), nullptr);
    for (const auto name : { "value", "flag", "caption", "enabled" }) {
        const auto* binding = findImportedNode(result, name);
        ASSERT_NE(binding, nullptr);
        EXPECT_EQ(binding->getName(), name);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, LoadsIndirectImportsWithoutDeclaringTheirBindings) {
    writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"External":{"category":"imported","source":"extra.tools.enabled"}})");

    const auto result = analyze("import pkg.api.value\nlet enabled = 0\nlet tools = 0\nlet extra = 0\nexport value, enabled, tools, extra\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* importedModule = package->getModuleByName("api");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(findImportedNode(result, "value"), importedModule->getIdentifierByName("value"));
    const auto* extraPackage = result.getImportedPackageByName("extra");
    ASSERT_NE(extraPackage, nullptr);
    const auto* toolsModule = extraPackage->getModuleByName("tools");
    ASSERT_NE(toolsModule, nullptr);
    const auto* enabled = dynamic_cast<const VnlcImportedLet*>(toolsModule->getIdentifierByName("enabled"));
    ASSERT_NE(enabled, nullptr);
    EXPECT_EQ(enabled->getType(), "bool");
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

TEST_F(VnlcSemanticAnalyzerImportTest, ScansTypeDependenciesInEveryNestedIdentifierCollection) {
    writeFile("another_source/types.vni", R"({"Remote":{"category":"typealias","genericParameters":[],"originalType":"pkg.sub.other.Target"}})");
    writeFile("dependency_source/sub/other.vni", R"({"Target":{"category":"typealias","genericParameters":[],"originalType":"int"}})");
    for (
        const auto contents : {
            R"({"category":"let","type":"extra.types.Remote"})",
            R"({"category":"func","returnType":"extra.types.Remote","native":false,"parameters":{}})",
            R"({"category":"func","returnType":"void","native":false,"parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}})",
            R"({"category":"method","returnType":"extra.types.Remote","native":false,"static":false,"accessModifier":"public","parameters":{}})",
            R"({"category":"method","returnType":"void","native":false,"static":false,"accessModifier":"public","parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}})",
            R"({"category":"class","genericParameters":[],"properties":{},"methods":{},"baseClass":"extra.types.Remote","implementedInterfaces":[],"final":false})",
            R"({"category":"class","genericParameters":[],"properties":{},"methods":{},"baseClass":null,"implementedInterfaces":["extra.types.Remote"],"final":false})",
            R"({"category":"class","genericParameters":[],"properties":{"property":{"category":"property","type":"extra.types.Remote","static":false,"accessModifier":"public"}},"methods":{},"baseClass":null,"implementedInterfaces":[],"final":false})",
            R"({"category":"class","genericParameters":[],"properties":{},"methods":{"method":{"category":"method","returnType":"extra.types.Remote","native":false,"static":false,"accessModifier":"public","parameters":{}}},"baseClass":null,"implementedInterfaces":[],"final":false})",
            R"({"category":"class","genericParameters":[],"properties":{},"methods":{"method":{"category":"method","returnType":"void","native":false,"static":false,"accessModifier":"public","parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}}},"baseClass":null,"implementedInterfaces":[],"final":false})",
            R"({"category":"interface","genericParameters":[],"methods":{"method":{"category":"method","returnType":"extra.types.Remote","native":false,"static":false,"accessModifier":"public","parameters":{}}}})",
            R"({"category":"interface","genericParameters":[],"methods":{"method":{"category":"method","returnType":"void","native":false,"static":false,"accessModifier":"public","parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}}}})",
            R"({"category":"enum","genericParameters":[],"members":{"member":{"category":"enummember","associatedValues":{"value":{"category":"enumvalue","type":"extra.types.Remote"}}}}})",
            R"({"category":"enummember","associatedValues":{"value":{"category":"enumvalue","type":"extra.types.Remote"}}})",
            R"({"category":"typealias","genericParameters":[],"originalType":"extra.types.Remote"})",
            R"({"category":"property","type":"extra.types.Remote","static":false,"accessModifier":"public"})",
            R"({"category":"parameter","type":"extra.types.Remote"})",
        }) {
        SCOPED_TRACE(contents);
        writeFile("dependency_source/api.vni", std::string(R"({"value":{"category":"let","type":"int"},"holder":)") + contents + "}");

        const auto result = analyze("import pkg.api.value\nexport value\n");

        ASSERT_FALSE(result.hasErrors());
        const auto* extraPackage = result.getImportedPackageByName("extra");
        ASSERT_NE(extraPackage, nullptr);
        const auto* typesModule = extraPackage->getModuleByName("types");
        ASSERT_NE(typesModule, nullptr);
        EXPECT_NE(typesModule->getIdentifierByName("Remote"), nullptr);
        const auto* package = result.getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* subPackage = package->getSubPackageByName("sub");
        ASSERT_NE(subPackage, nullptr);
        const auto* otherModule = subPackage->getModuleByName("other");
        ASSERT_NE(otherModule, nullptr);
        EXPECT_NE(otherModule->getIdentifierByName("Target"), nullptr);
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_NE(scope->lookupLocal("value"), nullptr);
        for (const auto name : { "holder", "Remote", "Target", "types", "other", "extra" }) {
            EXPECT_EQ(scope->lookupLocal(name), nullptr);
        }
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, LoadsEveryQualifiedTypeInGenericArgumentsAndNullableTypes) {
    writeFile("dependency_source/api.vni", R"({"Alias":{"category":"typealias","genericParameters":["T"],"originalType":"pkg.sub.other.Pair<T, List<extra.types.Remote?>>?"}})");
    writeFile("dependency_source/sub/other.vni", R"({"Pair":{"category":"typealias","genericParameters":["A","B"],"originalType":"A"}})");
    writeFile("another_source/types.vni", R"({"Remote":{"category":"typealias","genericParameters":[],"originalType":"int"}})");

    const auto result = analyze("import pkg.api.Alias\nexport Alias\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* subPackage = package->getSubPackageByName("sub");
    ASSERT_NE(subPackage, nullptr);
    const auto* otherModule = subPackage->getModuleByName("other");
    ASSERT_NE(otherModule, nullptr);
    EXPECT_NE(otherModule->getIdentifierByName("Pair"), nullptr);
    const auto* extraPackage = result.getImportedPackageByName("extra");
    ASSERT_NE(extraPackage, nullptr);
    const auto* typesModule = extraPackage->getModuleByName("types");
    ASSERT_NE(typesModule, nullptr);
    EXPECT_NE(typesModule->getIdentifierByName("Remote"), nullptr);
    const auto* scope = result.getScopeByAstNode(*module);
    ASSERT_NE(scope, nullptr);
    for (const auto name : { "Pair", "Remote", "T", "A", "B", "List" }) {
        EXPECT_EQ(scope->lookupLocal(name), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, FollowsTransitiveRepeatedAndCyclicIndirectImports) {
    writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"pkg.sub.other.bridge"},"repeated":{"category":"imported","source":"pkg.sub.other.bridge"}})");
    writeFile("dependency_source/sub/other.vni", R"({"bridge":{"category":"imported","source":"extra.tools.enabled"},"back":{"category":"imported","source":"pkg.api.External"}})");

    const auto result = analyze("import pkg.api.External\nexport External\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* importedModule = package->getModuleByName("api");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(findImportedNode(result, "External"), importedModule->getIdentifierByName("External"));
    const auto* subPackage = package->getSubPackageByName("sub");
    ASSERT_NE(subPackage, nullptr);
    const auto* otherModule = subPackage->getModuleByName("other");
    ASSERT_NE(otherModule, nullptr);
    EXPECT_NE(otherModule->getIdentifierByName("bridge"), nullptr);
    EXPECT_NE(otherModule->getIdentifierByName("back"), nullptr);
    const auto* extraPackage = result.getImportedPackageByName("extra");
    ASSERT_NE(extraPackage, nullptr);
    EXPECT_NE(extraPackage->getModuleByName("tools"), nullptr);
    const auto* scope = result.getScopeByAstNode(*module);
    ASSERT_NE(scope, nullptr);
    for (const auto name : { "repeated", "bridge", "back", "enabled", "other", "tools" }) {
        EXPECT_EQ(scope->lookupLocal(name), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, LoadsPackagesAndModulesReferencedByIndirectImports) {
    for (const auto source : { "extra", "extra.tools", "pkg.sub" }) {
        SCOPED_TRACE(source);
        writeFile("dependency_source/api.vni", std::string(R"({"External":{"category":"imported","source":")") + source + R"("}})");

        const auto result = analyze("import pkg.api.External\nexport External\n");

        ASSERT_FALSE(result.hasErrors());
        if (std::string_view(source) == "pkg.sub") {
            const auto* package = result.getImportedPackageByName("pkg");
            ASSERT_NE(package, nullptr);
            const auto* subPackage = package->getSubPackageByName("sub");
            ASSERT_NE(subPackage, nullptr);
            EXPECT_NE(subPackage->getModuleByName("other"), nullptr);
            EXPECT_NE(subPackage->getModuleByName("second"), nullptr);
        } else {
            const auto* extraPackage = result.getImportedPackageByName("extra");
            ASSERT_NE(extraPackage, nullptr);
            EXPECT_NE(extraPackage->getModuleByName("tools"), nullptr);
        }
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        for (const auto name : { "extra", "tools", "sub", "other", "second" }) {
            EXPECT_EQ(scope->lookupLocal(name), nullptr);
        }
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, PreservesBindingsWhenDirectAndIndirectImportsShareModules) {
    writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"extra.tools.enabled"}})");
    writeFile("dependency_source/sub/other.vni", R"({"flag":{"category":"let","type":"bool"},"External":{"category":"imported","source":"extra.tools.enabled"}})");

    for (const auto source : {
             "import pkg.api.External\nimport extra.tools.enabled\nimport pkg.sub.other.flag\nimport extra.tools as tools\nexport External, enabled, flag, tools\n",
             "import extra.tools.enabled\nimport pkg.api.External\nimport pkg.sub.other.flag\nimport extra.tools as tools\nexport External, enabled, flag, tools\n",
         }) {
        SCOPED_TRACE(source);
        const auto result = analyze(source);

        ASSERT_FALSE(result.hasErrors());
        const auto* package = result.getImportedPackageByName("pkg");
        ASSERT_NE(package, nullptr);
        const auto* importedModule = package->getModuleByName("api");
        ASSERT_NE(importedModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "External"), importedModule->getIdentifierByName("External"));
        const auto* subPackage = package->getSubPackageByName("sub");
        ASSERT_NE(subPackage, nullptr);
        const auto* otherModule = subPackage->getModuleByName("other");
        ASSERT_NE(otherModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "flag"), otherModule->getIdentifierByName("flag"));
        const auto* extraPackage = result.getImportedPackageByName("extra");
        ASSERT_NE(extraPackage, nullptr);
        const auto* toolsModule = extraPackage->getModuleByName("tools");
        ASSERT_NE(toolsModule, nullptr);
        EXPECT_EQ(findImportedNode(result, "tools"), toolsModule);
        EXPECT_EQ(findImportedNode(result, "enabled"), toolsModule->getIdentifierByName("enabled"));
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, DoesNotCommitImportsWithMissingOrMalformedIndirectDependencies) {
    writeFile("another_source/broken.vni", "invalid json");
    for (const auto source : { "absent.tools.enabled", "extra.missing.enabled", "extra.broken.enabled", "", ".extra.tools.enabled", "extra..tools.enabled", "extra.tools.enabled." }) {
        SCOPED_TRACE(source);
        writeFile(
            "dependency_source/api.vni",
            std::string(R"({"External":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":")") + source + R"("}})"
        );
        std::optional<VnlcSemanticAnalysisResult> result;
        ASSERT_NO_THROW(result.emplace(analyze("import pkg.api.External\nlet External = 0\n")));

        ASSERT_TRUE(result->hasErrors());
        ASSERT_EQ(result->getErrors().size(), 1);
        EXPECT_EQ(result->getImportedPackageByName("pkg"), nullptr);
        EXPECT_EQ(result->getImportedPackageByName("extra"), nullptr);
        EXPECT_EQ(result->getImportedPackageByName("absent"), nullptr);
        EXPECT_EQ(findImportedNode(*result, "External"), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, FailedIndirectImportsPreserveEarlierBindingsWithoutCommittingDependencies) {
    writeFile("dependency_source/broken.vni", R"({"External":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"pkg.sub.missing.value"}})");

    const auto result = analyze("import pkg.api as kept\nimport pkg.broken as staged\nlet staged = 0\nexport kept\n");

    ASSERT_TRUE(result.hasErrors());
    ASSERT_EQ(result.getErrors().size(), 1);
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* importedModule = package->getModuleByName("api");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(findImportedNode(result, "kept"), importedModule);
    EXPECT_EQ(package->getModuleByName("broken"), nullptr);
    EXPECT_EQ(package->getSubPackageByName("sub"), nullptr);
    EXPECT_EQ(result.getImportedPackageByName("extra"), nullptr);
    EXPECT_EQ(findImportedNode(result, "staged"), nullptr);
}

TEST_F(VnlcSemanticAnalyzerImportTest, ReportsInvalidImportsWithoutRetainingPartialTrees) {
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
        std::optional<VnlcSemanticAnalysisResult> result;
        ASSERT_NO_THROW(result.emplace(analyze(source)));

        ASSERT_TRUE(result->hasErrors());
        EXPECT_EQ(result->getImportedPackageByName("pkg"), nullptr);
        EXPECT_EQ(result->getImportedPackageByName("absent"), nullptr);
        EXPECT_EQ(findImportedNode(*result, "api"), nullptr);
        EXPECT_EQ(findImportedNode(*result, "value"), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, ReportsMalformedModuleInterfacesAsSemanticErrors) {
    for (const auto contents : { "invalid json", "[]", R"({"value":{"category":"let","type":123}})", R"({"value":{"category":null}})" }) {
        SCOPED_TRACE(contents);
        writeFile("dependency_source/broken.vni", contents);
        std::optional<VnlcSemanticAnalysisResult> result;
        ASSERT_NO_THROW(result.emplace(analyze("import pkg.broken\n")));

        ASSERT_TRUE(result->hasErrors());
        EXPECT_EQ(result->getImportedPackageByName("pkg"), nullptr);
        EXPECT_EQ(findImportedNode(*result, "broken"), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, RejectsMalformedUnselectedIdentifiersWhenImportingAModuleOrIdentifier) {
    writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"unused":{"category":"let","type":123}})");
    for (const auto source : { "import pkg.api\n", "import pkg.api.value\n" }) {
        SCOPED_TRACE(source);
        const auto result = analyze(source);

        ASSERT_TRUE(result.hasErrors());
        EXPECT_EQ(result.getImportedPackageByName("pkg"), nullptr);
        const auto* scope = result.getScopeByAstNode(*module);
        ASSERT_NE(scope, nullptr);
        EXPECT_EQ(scope->lookupLocal("api"), nullptr);
        EXPECT_EQ(scope->lookupLocal("value"), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, ReportsMissingDependencyDirectoriesAsSemanticErrors) {
    std::filesystem::remove_all(testDirectory / "dependency_source");
    std::optional<VnlcSemanticAnalysisResult> result;
    ASSERT_NO_THROW(result.emplace(analyze("import pkg.api\n")));

    ASSERT_TRUE(result->hasErrors());
    EXPECT_EQ(result->getImportedPackageByName("pkg"), nullptr);
    EXPECT_EQ(findImportedNode(*result, "api"), nullptr);
}

TEST_F(VnlcSemanticAnalyzerImportTest, RejectsDuplicateBindingsWithinOneImport) {
    for (const auto source : {
             "import pkg.api.{value, value}\n",
             "import pkg.{api.value as same, sub.other.flag as same}\n",
             "import pkg.api.{*, value}\n",
             "import pkg.api.{value, *}\n",
         }) {
        SCOPED_TRACE(source);
        const auto result = analyze(source);

        ASSERT_TRUE(result.hasErrors());
        EXPECT_EQ(result.getImportedPackageByName("pkg"), nullptr);
        EXPECT_EQ(findImportedNode(result, "value"), nullptr);
        EXPECT_EQ(findImportedNode(result, "same"), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, RejectsRedeclarationsAcrossImportsAndLocalDeclarations) {
    for (const auto source : {
             "import pkg.api.value\nimport pkg.api.value\n",
             "import pkg.api.value\nimport pkg.sub.other.flag as value\n",
             "import pkg.api.value\nlet value = 0\n",
         }) {
        SCOPED_TRACE(source);
        const auto result = analyze(source);

        ASSERT_TRUE(result.hasErrors());
        EXPECT_NE(findImportedNode(result, "value"), nullptr);
        EXPECT_NE(result.getImportedPackageByName("pkg"), nullptr);
    }
}

TEST_F(VnlcSemanticAnalyzerImportTest, FailedImportsPreserveEarlierBindingsAndDoNotCommitNewBranches) {
    const auto result = analyze("import pkg.api as kept\nimport pkg.sub.{other as staged, missing}\nlet staged = 0\nexport kept\n");

    ASSERT_TRUE(result.hasErrors());
    ASSERT_EQ(result.getErrors().size(), 1);
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* importedModule = package->getModuleByName("api");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(findImportedNode(result, "kept"), importedModule);
    EXPECT_EQ(package->getSubPackageByName("sub"), nullptr);
    EXPECT_EQ(findImportedNode(result, "staged"), nullptr);
}

TEST_F(VnlcSemanticAnalyzerImportTest, PreservesModuleNamesWhenReadingSymbolicLinks) {
    writeFile("shared/original.vni", R"({"linked":{"category":"let","type":"int"}})");
    std::error_code error;
    std::filesystem::create_symlink(testDirectory / "shared/original.vni", testDirectory / "dependency_source/linkedApi.vni", error);
    if (error) {
        GTEST_SKIP() << error.message();
    }

    const auto result = analyze("import pkg.linkedApi.{self, linked}\nexport linkedApi, linked\n");

    ASSERT_FALSE(result.hasErrors());
    const auto* package = result.getImportedPackageByName("pkg");
    ASSERT_NE(package, nullptr);
    const auto* importedModule = package->getModuleByName("linkedApi");
    ASSERT_NE(importedModule, nullptr);
    EXPECT_EQ(importedModule->getName(), "linkedApi");
    EXPECT_EQ(findImportedNode(result, "linkedApi"), importedModule);
    EXPECT_EQ(findImportedNode(result, "linked"), importedModule->getIdentifierByName("linked"));
}

TEST_F(VnlcSemanticAnalyzerImportTest, ReportsCyclicPackageDirectoriesAsSemanticErrors) {
    std::error_code error;
    std::filesystem::create_directory_symlink(testDirectory / "dependency_source", testDirectory / "dependency_source/loop", error);
    if (error) {
        GTEST_SKIP() << error.message();
    }

    std::optional<VnlcSemanticAnalysisResult> result;
    ASSERT_NO_THROW(result.emplace(analyze("import pkg\n")));

    ASSERT_TRUE(result->hasErrors());
    EXPECT_EQ(result->getImportedPackageByName("pkg"), nullptr);
    EXPECT_EQ(findImportedNode(*result, "pkg"), nullptr);
}
