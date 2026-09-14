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
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
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
            ASSERT_TRUE(context.currentScope().declare(
                VnlcSymbol(VnlcSymbolKind::CLASS, VnlcSymbolOrigin::LOCAL, VnlcSymbolAccessModifier::PUBLIC, classDeclaration->getName().getIdentifierString(), classDeclaration)
            ));
        }
        accessModules.push_back(parseModule("let privateMember = 0\n", config));
        ASSERT_TRUE(context.currentScope().declare(
            VnlcSymbol(VnlcSymbolKind::VARIABLE, VnlcSymbolOrigin::LOCAL, VnlcSymbolAccessModifier::PUBLIC, "privateMember", accessModules.back()->getTopIdentifierDeclarations().front().get())
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
        return analyzer->context.getCustomizedTypeByFullTypeName(std::string(module->getFullName()) + "." + std::string(name)).value_or(nullptr);
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
        const auto* parent = context.getScopeByAstNode(accessor.empty() ? static_cast<const VnlcAstNode*>(module.get()) : findType(accessor)->getLocalDeclaration());
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
            EXPECT_TRUE(context.currentScope().declare(
                VnlcSymbol(VnlcSymbolKind::PARAMETER, VnlcSymbolOrigin::LOCAL, VnlcSymbolAccessModifier::PUBLIC, identifier, function.getParameters().front().get())
            ));
        }
        if (nestedBlock) context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::BLOCK, &context.currentScope(), &block));
        if (localSymbolKind == VnlcSymbolKind::VARIABLE) {
            const auto& declaration = dynamic_cast<const VnlcVariableDeclarationStatementNode&>(*block.getStatements().front()).getVariableDeclaration();
            EXPECT_TRUE(context.currentScope().declare(VnlcSymbol(VnlcSymbolKind::VARIABLE, VnlcSymbolOrigin::LOCAL, VnlcSymbolAccessModifier::PUBLIC, identifier, &declaration)));
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
    EXPECT_FALSE(result.getCustomizedTypeByFullTypeName("semantic_test_package.models.registration_errors.InvalidClass").has_value());

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
    ASSERT_TRUE(context.currentScope().declare(VnlcSymbol(VnlcSymbolKind::CLASS, VnlcSymbolOrigin::LOCAL, VnlcSymbolAccessModifier::PUBLIC, "Sample", classDeclaration)));
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
    const auto symbol = classScope->lookup("Sample");
    ASSERT_TRUE(symbol.has_value());
    EXPECT_EQ(symbol.value()->getLocalDeclarationNode(), classDeclaration);
}
