#include "SemanticAnalyzer.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/EnumDeclarationNode.hpp"
#include "ast/declaration/EnumMemberDeclarationNode.hpp"
#include "ast/declaration/InterfaceDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeAliasDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"
#include "ast/expression/BinaryExpressionNode.hpp"
#include "ast/expression/ConditionalExpressionNode.hpp"
#include "ast/expression/DictLiteralExpressionNode.hpp"
#include "ast/expression/FunctionCallExpressionNode.hpp"
#include "ast/expression/IdentifierLikeExpressionNode.hpp"
#include "ast/expression/ListLikeLiteralExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionNode.hpp"
#include "ast/expression/NoneExpressionNode.hpp"
#include "ast/expression/PrimitiveTypeExpressionNode.hpp"
#include "ast/expression/RangeExpressionNode.hpp"
#include "ast/expression/SelectorLiteralExpressionNode.hpp"
#include "ast/expression/SimpleLiteralExpressionNode.hpp"
#include "ast/expression/StringLiteralExpressionNode.hpp"
#include "ast/expression/SubscriptExpressionNode.hpp"
#include "ast/expression/SuperExpressionNode.hpp"
#include "ast/expression/ThisExpressionNode.hpp"
#include "ast/expression/UnaryExpressionNode.hpp"
#include "ast/statement/BlockStatementNode.hpp"
#include "ast/statement/BreakStatementNode.hpp"
#include "ast/statement/ContinueStatementNode.hpp"
#include "ast/statement/ExpressionStatementNode.hpp"
#include "ast/statement/ForStatementNode.hpp"
#include "ast/statement/IfStatementNode.hpp"
#include "ast/statement/ReturnStatementNode.hpp"
#include "ast/statement/SwitchStatementNode.hpp"
#include "ast/statement/VariableDeclarationStatementNode.hpp"
#include "ast/statement/WhileStatementNode.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "ast/typeref/PrimitiveTypeReferenceNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include "symbol/RegularSymbol.hpp"
#include "symbol/RegularSymbolAccessModifier.hpp"
#include "symbol/RegularSymbolKind.hpp"
#include "symbol/RegularSymbolOrigin.hpp"
#include "type/CustomizedType.hpp"
#include "type/CustomizedTypeKind.hpp"
#include "type/PrimitiveType.hpp"
#include "type/Type.hpp"
#include "type/typeinf/TypeInferenceResult.hpp"
#include "vni/import/ImportedAlias.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedConstructor.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedEnumValue.hpp"
#include "vni/import/ImportedFunc.hpp"
#include "vni/import/ImportedIdentifier.hpp"
#include "vni/import/ImportedInterface.hpp"
#include "vni/import/ImportedItem.hpp"
#include "vni/import/ImportedLet.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedOperator.hpp"
#include "vni/import/ImportedParameter.hpp"
#include "vni/import/ImportedProperty.hpp"
#include "vni/import/ImportedTypeAlias.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <functional>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_set>

namespace vnlc {
    SemanticAnalyzer::SemanticAnalyzer(const ModuleNode& module, const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports) : module(module), imports(imports) {}

    RegularSymbolKind SemanticAnalyzer::getImportedSymbolKind(const ImportedItem& item) {
        if (dynamic_cast<const ImportedPackage*>(&item)) return RegularSymbolKind::PACKAGE;
        if (dynamic_cast<const ImportedModule*>(&item)) return RegularSymbolKind::MODULE;
        if (dynamic_cast<const ImportedLet*>(&item)) return RegularSymbolKind::VARIABLE;
        if (dynamic_cast<const ImportedFunc*>(&item)) return RegularSymbolKind::FUNCTION;
        if (dynamic_cast<const ImportedClass*>(&item)) return RegularSymbolKind::CLASS;
        if (dynamic_cast<const ImportedInterface*>(&item)) return RegularSymbolKind::INTERFACE;
        if (dynamic_cast<const ImportedEnum*>(&item)) return RegularSymbolKind::ENUM;
        if (dynamic_cast<const ImportedTypeAlias*>(&item)) return RegularSymbolKind::TYPE_ALIAS;
        if (dynamic_cast<const ImportedEnumMember*>(&item)) return RegularSymbolKind::ENUM_MEMBER;
        if (dynamic_cast<const ImportedProperty*>(&item) || dynamic_cast<const ImportedEnumValue*>(&item)) return RegularSymbolKind::PROPERTY;
        if (dynamic_cast<const ImportedMethod*>(&item)) return RegularSymbolKind::METHOD;
        if (dynamic_cast<const ImportedConstructor*>(&item)) return RegularSymbolKind::METHOD;
        if (dynamic_cast<const ImportedParameter*>(&item)) return RegularSymbolKind::PARAMETER;
        return RegularSymbolKind::IMPORT_ALIAS;
    }

    std::optional<ScopeKind> SemanticAnalyzer::getImportedScopeKind(const ImportedItem& item) {
        switch (getImportedSymbolKind(item)) {
            case RegularSymbolKind::PACKAGE:
                return ScopeKind::PACKAGE;
            case RegularSymbolKind::MODULE:
                return ScopeKind::MODULE;
            case RegularSymbolKind::CLASS:
                return ScopeKind::CLASS;
            case RegularSymbolKind::INTERFACE:
                return ScopeKind::INTERFACE;
            case RegularSymbolKind::ENUM:
                return ScopeKind::ENUM;
            case RegularSymbolKind::ENUM_MEMBER:
                return ScopeKind::ENUM_MEMBER;
            case RegularSymbolKind::TYPE_ALIAS:
                return ScopeKind::TYPE_ALIAS;
            case RegularSymbolKind::FUNCTION:
            case RegularSymbolKind::METHOD:
                return ScopeKind::FUNCTION;
            default:
                return std::nullopt;
        }
    }

    RegularSymbolAccessModifier SemanticAnalyzer::getImportedAccessModifier(const ImportedItem& item) {
        std::string_view accessModifier;
        if (const auto* property = dynamic_cast<const ImportedProperty*>(&item)) {
            accessModifier = property->getAccessModifier();
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&item)) {
            accessModifier = method->getAccessModifier();
        } else if (const auto* constructor = dynamic_cast<const ImportedConstructor*>(&item)) {
            accessModifier = constructor->getAccessModifier();
        }
        if (accessModifier == "private") return RegularSymbolAccessModifier::PRIVATE;
        if (accessModifier == "protected") return RegularSymbolAccessModifier::PROTECTED;
        return RegularSymbolAccessModifier::PUBLIC;
    }

    const ImportedPackage* SemanticAnalyzer::getImportedRootPackageByName(std::string_view name) const {
        auto it = imports.find(std::string(name));
        if (it != imports.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    const Type* SemanticAnalyzer::registerImportedTypeReference(std::string_view fullTypeName) {
        const auto isSpace = [](char character) { return character == ' ' || character == '\t' || character == '\r' || character == '\n'; };
        const auto trim = [&isSpace](std::string_view value) {
            while (!value.empty() && isSpace(value.front())) {
                value.remove_prefix(1);
            }
            while (!value.empty() && isSpace(value.back())) {
                value.remove_suffix(1);
            }
            return value;
        };

        fullTypeName = trim(fullTypeName);
        if (fullTypeName.empty()) {
            return nullptr;
        }

        const std::string fullName(fullTypeName);
        if (const CustomizedType* existingType = context.getCustomizedTypeByFullTypeName(fullName)) {
            return existingType;
        }

        const std::size_t genericArgumentStart = fullTypeName.find('<');
        std::string_view unwrappedTypeName = trim(fullTypeName.substr(0, genericArgumentStart));

        if (const PrimitiveType* primitiveType = PrimitiveType::getPrimitiveTypeByFullTypeName(unwrappedTypeName)) {
            return primitiveType;
        }

        std::vector<const Type*> genericArgumentTypes;
        if (genericArgumentStart != std::string_view::npos) {
            std::size_t depth = 0;
            std::size_t genericArgumentEnd = std::string_view::npos;
            for (std::size_t index = genericArgumentStart; index < fullTypeName.size(); ++index) {
                if (fullTypeName[index] == '<') {
                    ++depth;
                } else if (fullTypeName[index] == '>') {
                    --depth;
                    if (depth == 0) {
                        genericArgumentEnd = index;
                        break;
                    }
                }
            }

            if (genericArgumentEnd == std::string_view::npos) {
                return nullptr;
            }

            std::size_t argumentStart = genericArgumentStart + 1;
            depth = 0;
            for (std::size_t index = argumentStart; index < genericArgumentEnd; ++index) {
                if (fullTypeName[index] == '<') {
                    ++depth;
                } else if (fullTypeName[index] == '>') {
                    --depth;
                } else if (fullTypeName[index] == ',' && depth == 0) {
                    const std::string_view argument = trim(fullTypeName.substr(argumentStart, index - argumentStart));
                    const Type* argumentType = registerImportedTypeReference(argument);
                    if (argumentType == nullptr) {
                        return nullptr;
                    }
                    genericArgumentTypes.emplace_back(argumentType);
                    argumentStart = index + 1;
                }
            }

            const std::string_view lastArgument = trim(fullTypeName.substr(argumentStart, genericArgumentEnd - argumentStart));
            const Type* lastArgumentType = registerImportedTypeReference(lastArgument);
            if (lastArgumentType == nullptr) {
                return nullptr;
            }
            genericArgumentTypes.emplace_back(lastArgumentType);
        }

        std::vector<std::string> path;
        std::size_t start = 0;
        while (start <= unwrappedTypeName.size()) {
            const std::size_t separator = unwrappedTypeName.find('.', start);
            if (separator == std::string_view::npos) {
                path.emplace_back(unwrappedTypeName.substr(start));
                break;
            }

            path.emplace_back(unwrappedTypeName.substr(start, separator - start));
            start = separator + 1;
        }

        const ImportedItem* importedType = ImportedItem::getImportedItemByFullPath(imports, path);

        // imported type references should never be type aliases
        const auto customizedTypeKind = [&importedType]() -> std::optional<CustomizedTypeKind> {
            if (dynamic_cast<const ImportedClass*>(importedType) != nullptr) return CustomizedTypeKind::CLASS;
            if (dynamic_cast<const ImportedInterface*>(importedType) != nullptr) return CustomizedTypeKind::INTERFACE;
            if (dynamic_cast<const ImportedEnum*>(importedType) != nullptr) return CustomizedTypeKind::ENUM;
            if (dynamic_cast<const ImportedEnumMember*>(importedType) != nullptr) return CustomizedTypeKind::ENUM_MEMBER;
            return std::nullopt;
        }();
        if (!customizedTypeKind.has_value()) {
            return nullptr;
        }

        const auto* importedIdentifier = dynamic_cast<const ImportedIdentifier*>(importedType);
        if (importedIdentifier == nullptr) {
            return nullptr;
        }

        auto customizedType = std::make_unique<CustomizedType>(customizedTypeKind.value(), fullName, std::move(genericArgumentTypes), importedIdentifier);
        const Type* type = customizedType.get();
        context.registerCustomizedType(std::move(customizedType));
        return type;
    }

    void SemanticAnalyzer::registerImportedTypeReferences(const ImportedItem& importedItem) {
        const auto registerTypeReference = [&](std::string_view fullTypeName) { registerImportedTypeReference(fullTypeName); };
        const auto registerChildren = [&](const auto& children) {
            for (const auto& [name, child] : children) {
                registerImportedTypeReferences(*child);
            }
        };
        const auto registerParameters = [&](const auto& parameters) {
            for (const auto& [name, parameter] : parameters) {
                registerTypeReference(parameter->getType());
            }
        };

        if (const auto* let = dynamic_cast<const ImportedLet*>(&importedItem)) {
            registerTypeReference(let->getType());
        } else if (const auto* function = dynamic_cast<const ImportedFunc*>(&importedItem)) {
            registerTypeReference(function->getReturnType());
            registerParameters(function->getParameters());
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&importedItem)) {
            registerTypeReference(method->getReturnType());
            registerParameters(method->getParameters());
        } else if (const auto* operatorNode = dynamic_cast<const ImportedOperator*>(&importedItem)) {
            registerTypeReference(operatorNode->getReturnType());
            registerParameters(operatorNode->getParameters());
        } else if (const auto* constructor = dynamic_cast<const ImportedConstructor*>(&importedItem)) {
            registerParameters(constructor->getParameters());
        } else if (const auto* property = dynamic_cast<const ImportedProperty*>(&importedItem)) {
            registerTypeReference(property->getType());
        } else if (const auto* parameter = dynamic_cast<const ImportedParameter*>(&importedItem)) {
            registerTypeReference(parameter->getType());
        } else if (const auto* classType = dynamic_cast<const ImportedClass*>(&importedItem)) {
            if (classType->getBaseClass().has_value()) {
                registerTypeReference(classType->getBaseClass().value());
            }
            for (const auto& interfaceName : classType->getImplementedInterfaces()) {
                registerTypeReference(interfaceName);
            }
            registerChildren(classType->getProperties());
            registerChildren(classType->getMethods());
            registerChildren(classType->getConstructors());
            registerChildren(classType->getOperators());
        } else if (const auto* interfaceType = dynamic_cast<const ImportedInterface*>(&importedItem)) {
            registerChildren(interfaceType->getMethods());
            registerChildren(interfaceType->getOperators());
        } else if (const auto* enumType = dynamic_cast<const ImportedEnum*>(&importedItem)) {
            registerChildren(enumType->getMembers());
        } else if (const auto* enumMember = dynamic_cast<const ImportedEnumMember*>(&importedItem)) {
            registerChildren(enumMember->getAssociatedValues());
        } else if (const auto* enumValue = dynamic_cast<const ImportedEnumValue*>(&importedItem)) {
            registerTypeReference(enumValue->getType());
        } else if (const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(&importedItem)) {
            registerTypeReference(typeAlias->getOriginalType());
        } else if (const auto* module = dynamic_cast<const ImportedModule*>(&importedItem)) {
            registerChildren(module->getIdentifiers());
        } else if (const auto* package = dynamic_cast<const ImportedPackage*>(&importedItem)) {
            registerChildren(package->getSubPackages());
            registerChildren(package->getModules());
        }
    }

    void SemanticAnalyzer::registerImportedScopes(const ImportedItem& item, const Scope* parent) {
        const auto kind = getImportedScopeKind(item);
        if (!kind.has_value()) return;
        auto& scope = context.getOrCreateImportedScope(kind.value(), parent, item);
        const auto declareChildren = [&](const auto& children) {
            for (const auto& [name, child] : children) {
                registerImportedScopes(*child, &scope);

                if (dynamic_cast<const ImportedOperator*>(child.get()) != nullptr) continue;
                if (dynamic_cast<const ImportedConstructor*>(child.get()) != nullptr) continue;
                if (const auto* function = dynamic_cast<const ImportedFunc*>(child.get())) {
                    FunctionSymbol functionSymbol(function->getOriginalName());
                    functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::FUNCTION, getImportedAccessModifier(*child), child->getName(), child.get()));
                    scope.declare(std::move(functionSymbol));
                } else if (const auto* method = dynamic_cast<const ImportedMethod*>(child.get())) {
                    FunctionSymbol functionSymbol(method->getOriginalName());
                    functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::METHOD, getImportedAccessModifier(*child), child->getName(), child.get()));
                    scope.declare(std::move(functionSymbol));
                } else {
                    scope.declare(RegularSymbol(getImportedSymbolKind(*child), getImportedAccessModifier(*child), name, child.get()));
                }
            }
        };
        const auto declareGenericParameters = [&](const auto& parameters) {
            for (const auto& name : parameters) {
                scope.declare(RegularSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, name, &item));
            }
        };

        if (const auto* package = dynamic_cast<const ImportedPackage*>(&item)) {
            declareChildren(package->getSubPackages());
            declareChildren(package->getModules());
        } else if (const auto* module = dynamic_cast<const ImportedModule*>(&item)) {
            declareChildren(module->getIdentifiers());
        } else if (const auto* classType = dynamic_cast<const ImportedClass*>(&item)) {
            declareChildren(classType->getProperties());
            declareChildren(classType->getMethods());
            declareChildren(classType->getConstructors());
            declareChildren(classType->getOperators());
            declareGenericParameters(classType->getGenericParameters());
        } else if (const auto* interfaceType = dynamic_cast<const ImportedInterface*>(&item)) {
            declareChildren(interfaceType->getMethods());
            declareChildren(interfaceType->getOperators());
            declareGenericParameters(interfaceType->getGenericParameters());
        } else if (const auto* enumType = dynamic_cast<const ImportedEnum*>(&item)) {
            declareChildren(enumType->getMembers());
            declareGenericParameters(enumType->getGenericParameters());
        } else if (const auto* enumMember = dynamic_cast<const ImportedEnumMember*>(&item)) {
            declareChildren(enumMember->getAssociatedValues());
        } else if (const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(&item)) {
            declareGenericParameters(typeAlias->getGenericParameters());
        } else if (const auto* function = dynamic_cast<const ImportedFunc*>(&item)) {
            declareChildren(function->getParameters());
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&item)) {
            declareChildren(method->getParameters());
        } else if (const auto* constructor = dynamic_cast<const ImportedConstructor*>(&item)) {
            declareChildren(constructor->getParameters());
        }
    }

    void SemanticAnalyzer::checkIdentifierExpressionUse(const IdentifierLikeExpressionNode& exprNode, MetadataInfo metadataInfo) {
        const Symbol* symbol = context.currentScope().lookup(exprNode.getName().getIdentifierString());
        if (symbol == nullptr) {
            context.reportError(exprNode, fmt::format("Use of undeclared identifier '{}'", exprNode.getName().getIdentifierString()));
        } else if (dynamic_cast<const FunctionSymbol*>(symbol) != nullptr) {
            return;
        } else if (
            const auto* regularSymbol = dynamic_cast<const RegularSymbol*>(symbol); regularSymbol == nullptr || !(dynamic_cast<const ValueDeclarationNode*>(regularSymbol->getLocalNode()) ||
                                                                                                                  dynamic_cast<const FunctionDeclarationNode*>(regularSymbol->getLocalNode()))
        ) {
            context.reportError(exprNode, fmt::format("Identifier '{}' is not a variable or function", exprNode.getName().getIdentifierString()));
        }
    }

    MetadataInfo SemanticAnalyzer::checkMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms, const DeclarationNode& declNode) {
        bool noWarnings = false;
        bool deprecated = false;

        for (const auto& term : metadataTerms) {
            if (term.key->getIdentifierString() == "nowarnings") {
                noWarnings = true;
            } else if (term.key->getIdentifierString() == "deprecated") {
                deprecated = true;
            } else {
                context.reportError(declNode, fmt::format("Unknown metadata term '{}'", term.key->getIdentifierString()));
            }
        }

        return MetadataInfo{
            noWarnings,
            deprecated,
        };
    }

    void SemanticAnalyzer::declareFunctionOverloading(const FunctionDeclarationNode& funcDecl, RegularSymbolKind kind, std::string_view declarationKind) {
        const auto name = funcDecl.getName().getIdentifierString();
        const auto internalName = funcDecl.getInternalName();
        const Symbol* existingSymbol = context.currentScope().lookupLocal(name);

        if (existingSymbol != nullptr) {
            const auto* existingFunction = dynamic_cast<const FunctionSymbol*>(existingSymbol);
            if (existingFunction == nullptr) {
                context.reportError(funcDecl, fmt::format("Redeclaration of {} '{}'", declarationKind, name));
                return;
            }
            if (existingFunction->getOverloadingByInternalName(internalName) != nullptr) {
                context.reportError(funcDecl, fmt::format("Redeclaration of {} '{}'", declarationKind, internalName));
                return;
            }
        }

        FunctionSymbol functionSymbol(name);
        functionSymbol.addOverloading(RegularSymbol(kind, static_cast<RegularSymbolAccessModifier>(funcDecl.getAccessModifier()), internalName, &funcDecl));

        if (!context.currentScope().declare(std::move(functionSymbol))) {
            context.reportError(funcDecl, fmt::format("Redeclaration of {} '{}'", declarationKind, name));
        }
    }

    void SemanticAnalyzer::collectLocalSymbolsInModule(const ModuleNode& moduleNode) {
        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::VARIABLE, static_cast<RegularSymbolAccessModifier>(varDecl->getAccessModifier()), varDecl->getName().getIdentifierString(), varDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*varDecl, fmt::format("Redeclaration of symbol '{}'", varDecl->getName().getIdentifierString()));
                }
            } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::CLASS, RegularSymbolAccessModifier::PUBLIC, classDecl->getName().getIdentifierString(), classDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*classDecl, fmt::format("Redeclaration of symbol '{}'", classDecl->getName().getIdentifierString()));
                }
            } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::INTERFACE, RegularSymbolAccessModifier::PUBLIC, interfaceDecl->getName().getIdentifierString(), interfaceDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*interfaceDecl, fmt::format("Redeclaration of symbol '{}'", interfaceDecl->getName().getIdentifierString()));
                }
            } else if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::ENUM, RegularSymbolAccessModifier::PUBLIC, enumDecl->getName().getIdentifierString(), enumDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*enumDecl, fmt::format("Redeclaration of symbol '{}'", enumDecl->getName().getIdentifierString()));
                }
            } else if (auto* typeAliasDecl = dynamic_cast<TypeAliasDeclarationNode*>(declNode)) {
                RegularSymbol symbol(RegularSymbolKind::TYPE_ALIAS, RegularSymbolAccessModifier::PUBLIC, typeAliasDecl->getAliasName().getIdentifierString(), typeAliasDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*typeAliasDecl, fmt::format("Redeclaration of symbol '{}'", typeAliasDecl->getAliasName().getIdentifierString()));
                }
            }
        }

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declNode)) {
                collectLocalSymbolsInEnum(*enumDecl);
            } else if (auto* typeAliasDecl = dynamic_cast<TypeAliasDeclarationNode*>(declNode)) {
                collectLocalSymbolsInTypeAlias(*typeAliasDecl);
            }
        }

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            if (const auto* funcDecl = dynamic_cast<const FunctionDeclarationNode*>(topIdentifierDecl.get())) {
                declareFunctionOverloading(*funcDecl, RegularSymbolKind::FUNCTION, "symbol");
            }
        }

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(declNode)) {
                collectLocalSymbolsInFunction(*funcDecl);
            } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declNode)) {
                collectLocalSymbolsInClass(*classDecl);
            } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declNode)) {
                collectLocalSymbolsInInterface(*interfaceDecl);
            }
        }
    }

    void SemanticAnalyzer::collectLocalSymbolsInFunction(const FunctionDeclarationNode& functionNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, &context.currentScope(), &functionNode));

        for (const auto& parameter : functionNode.getParameters()) {
            RegularSymbol parameterSymbol(RegularSymbolKind::PARAMETER, RegularSymbolAccessModifier::PUBLIC, parameter->getName().getIdentifierString(), parameter.get());
            if (!context.currentScope().declare(std::move(parameterSymbol))) {
                context.reportError(*parameter, fmt::format("Redeclaration of parameter '{}'", parameter->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalSymbolsInConstructor(const ConstructorDeclarationNode& constructorNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, &context.currentScope(), &constructorNode));

        for (const auto& parameter : constructorNode.getParameters()) {
            RegularSymbol parameterSymbol(RegularSymbolKind::PARAMETER, RegularSymbolAccessModifier::PUBLIC, parameter->getName().getIdentifierString(), parameter.get());
            if (!context.currentScope().declare(std::move(parameterSymbol))) {
                context.reportError(*parameter, fmt::format("Redeclaration of parameter '{}'", parameter->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalSymbolsInOperator(const OperatorDeclarationNode& operatorNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, &context.currentScope(), &operatorNode));

        for (const auto& parameter : operatorNode.getParameters()) {
            RegularSymbol parameterSymbol(RegularSymbolKind::PARAMETER, RegularSymbolAccessModifier::PUBLIC, parameter->getName().getIdentifierString(), parameter.get());
            if (!context.currentScope().declare(std::move(parameterSymbol))) {
                context.reportError(*parameter, fmt::format("Redeclaration of parameter '{}'", parameter->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalSymbolsInClass(const ClassDeclarationNode& classNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::CLASS, &context.currentScope(), &classNode));
        std::unordered_set<std::string> operatorNames;
        std::unordered_set<std::string> constructorNames;

        for (const auto& propertyDecl : classNode.getPropertyDeclarations()) {
            RegularSymbol memberSymbol(
                RegularSymbolKind::PROPERTY,
                static_cast<RegularSymbolAccessModifier>(propertyDecl->getAccessModifier()),
                propertyDecl->getName().getIdentifierString(),
                propertyDecl.get()
            );
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*propertyDecl, fmt::format("Redeclaration of class member '{}'", propertyDecl->getName().getIdentifierString()));
            }
        }

        for (const auto& genericParamName : classNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &classNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(classNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& methodDecl : classNode.getMethodDeclarations()) {
            declareFunctionOverloading(*methodDecl, RegularSymbolKind::METHOD, "class member");
        }

        for (const auto& constructorDecl : classNode.getConstructorDeclarations()) {
            if (!constructorNames.insert(std::string(constructorDecl->getInternalName())).second) {
                context.reportError(*constructorDecl, fmt::format("Redeclaration of class member '{}'", constructorDecl->getInternalName()));
            }
        }

        for (const auto& operatorDecl : classNode.getOperatorDeclarations()) {
            if (!operatorNames.insert(std::string(operatorDecl->getInternalName())).second) {
                context.reportError(*operatorDecl, fmt::format("Redeclaration of class member '{}'", operatorDecl->getInternalName()));
            }
        }

        for (const auto& methodDecl : classNode.getMethodDeclarations()) {
            collectLocalSymbolsInFunction(*methodDecl);
        }

        for (const auto& constructorDecl : classNode.getConstructorDeclarations()) {
            collectLocalSymbolsInConstructor(*constructorDecl);
        }

        for (const auto& operatorDecl : classNode.getOperatorDeclarations()) {
            collectLocalSymbolsInOperator(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalSymbolsInInterface(const InterfaceDeclarationNode& interfaceNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::INTERFACE, &context.currentScope(), &interfaceNode));
        std::unordered_set<std::string> operatorNames;

        for (const auto& genericParamName : interfaceNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &interfaceNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(interfaceNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& methodDecl : interfaceNode.getMethodDeclarations()) {
            declareFunctionOverloading(*methodDecl, RegularSymbolKind::METHOD, "interface method");
        }

        for (const auto& operatorDecl : interfaceNode.getOperatorDeclarations()) {
            if (!operatorNames.insert(std::string(operatorDecl->getInternalName())).second) {
                context.reportError(*operatorDecl, fmt::format("Redeclaration of interface operator '{}'", operatorDecl->getInternalName()));
            }
        }

        for (const auto& methodDecl : interfaceNode.getMethodDeclarations()) {
            collectLocalSymbolsInFunction(*methodDecl);
        }

        for (const auto& operatorDecl : interfaceNode.getOperatorDeclarations()) {
            collectLocalSymbolsInOperator(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalSymbolsInEnum(const EnumDeclarationNode& enumNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::ENUM, &context.currentScope(), &enumNode));

        for (const auto& member : enumNode.getMemberDeclarations()) {
            RegularSymbol memberSymbol(RegularSymbolKind::ENUM_MEMBER, RegularSymbolAccessModifier::PUBLIC, member->getName().getIdentifierString(), member.get());
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*member, fmt::format("Redeclaration of enum member '{}'", member->getName().getIdentifierString()));
            }
        }

        for (const auto& genericParamName : enumNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &enumNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(enumNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& member : enumNode.getMemberDeclarations()) {
            collectLocalSymbolsInEnumMember(*member);
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalSymbolsInEnumMember(const EnumMemberDeclarationNode& enumMemberNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::ENUM_MEMBER, &context.currentScope(), &enumMemberNode));

        for (const auto& associatedValue : enumMemberNode.getAssociatedValues()) {
            RegularSymbol associatedValueSymbol(RegularSymbolKind::PROPERTY, RegularSymbolAccessModifier::PUBLIC, associatedValue->getName().getIdentifierString(), associatedValue.get());
            if (!context.currentScope().declare(std::move(associatedValueSymbol))) {
                context.reportError(*associatedValue, fmt::format("Redeclaration of enum member associated value '{}'", associatedValue->getName().getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::collectLocalSymbolsInTypeAlias(const TypeAliasDeclarationNode& typeAliasNode) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::TYPE_ALIAS, &context.currentScope(), &typeAliasNode));

        for (const auto& genericParamName : typeAliasNode.getGenericParameterNames()) {
            RegularSymbol genericParamSymbol(RegularSymbolKind::GENERIC_PARAMETER, RegularSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &typeAliasNode);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(typeAliasNode, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkModule(const ModuleNode& moduleNode, const Config& config) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, &moduleNode));

        for (const auto& importDecl : moduleNode.getImportDeclarations()) {
            checkImport(*importDecl, config);
        }

        collectLocalSymbolsInModule(moduleNode);

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(declNode)) {
                if (varDecl->getKind() == ValueDeclarationKind::Kind::LET) {
                    checkValueDeclaration(*varDecl);
                } else {
                    context.reportError(*varDecl, fmt::format("Top-level value declaration must be a variable"));
                }
            } else if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(declNode)) {
                checkFunctionDeclaration(*funcDecl);
            } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declNode)) {
                checkClassDeclaration(*classDecl, config);
            } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declNode)) {
                checkInterfaceDeclaration(*interfaceDecl, config);
            } else if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declNode)) {
                checkEnumDeclaration(*enumDecl, config);
            } else if (auto* typeAliasDecl = dynamic_cast<TypeAliasDeclarationNode*>(declNode)) {
                checkTypeAliasDeclaration(*typeAliasDecl, config);
            }
        }

        for (const auto& exportDecl : moduleNode.getExportDeclarations()) {
            checkExport(*exportDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkImport(const ImportDeclarationNode& importDecl, const Config&) {
        const auto& importItem = importDecl.getNamePartsListWithAliases();

        struct ImportBinding {
            std::string name;
            std::vector<std::string> path;
        };

        std::vector<ImportBinding> bindings;
        std::vector<const ImportedItem*> importedRoots;
        std::unordered_map<std::string, bool> names;
        const auto errorCount = context.getErrors().size();

        const auto bind = [&](const ImportedItem* target, std::vector<std::string> path, const IdentifierNode* alias, const AstNode& location) {
            std::string name;
            if (alias != nullptr) {
                name = alias->getIdentifierString();
            } else if (const auto* function = dynamic_cast<const ImportedFunc*>(target)) {
                name = function->getOriginalName();
            } else if (const auto* method = dynamic_cast<const ImportedMethod*>(target)) {
                name = method->getOriginalName();
            } else {
                name = target->getName();
            }

            std::unordered_set<const ImportedAlias*> visitedAliases;
            while (const auto* importedAlias = dynamic_cast<const ImportedAlias*>(target)) {
                if (!visitedAliases.insert(importedAlias).second) {
                    context.reportError(location, fmt::format("Cyclic imported alias '{}'", importedAlias->getSource()));
                    return;
                }

                path.assign(1, std::string());
                for (const char ch : importedAlias->getSource()) {
                    if (ch == '.') {
                        path.emplace_back();
                    } else {
                        path.back().push_back(ch);
                    }
                }

                target = ImportedItem::getImportedItemByFullPath(imports, path);
                if (target == nullptr) {
                    context.reportError(location, fmt::format("Could not find imported alias target '{}'", importedAlias->getSource()));
                    return;
                }
            }

            const bool isFunction = dynamic_cast<const ImportedFunc*>(target) != nullptr || dynamic_cast<const ImportedMethod*>(target) != nullptr;

            const Symbol* existingSymbol = context.currentScope().lookupLocal(name);
            if (existingSymbol != nullptr && !(isFunction && dynamic_cast<const FunctionSymbol*>(existingSymbol) != nullptr)) {
                context.reportError(location, fmt::format("Redeclaration of symbol '{}'", name));
                return;
            }

            auto [nameIterator, inserted] = names.try_emplace(name, isFunction);
            if (!inserted && !(nameIterator->second && isFunction)) {
                context.reportError(location, fmt::format("Redeclaration of symbol '{}'", name));
                return;
            }

            bindings.push_back({ name, std::move(path) });
        };

        std::function<void(const ImportDeclarationItem&, const ImportedItem*, std::vector<std::string>)> checkItem;
        checkItem = [&](const ImportDeclarationItem& item, const ImportedItem* parent, std::vector<std::string> path) {
            const AstNode& location = item.namePrefix.empty() ? static_cast<const AstNode&>(importDecl) : *item.namePrefix.back();
            const ImportedItem* target = parent;
            if (!item.self) {
                for (const auto& part : item.namePrefix) {
                    const auto name = part->getIdentifierString();
                    if (item.wildcard && name == "*") {
                        continue;
                    }
                    const ImportedItem* parentItem = target;
                    if (target == nullptr) {
                        target = getImportedRootPackageByName(name);
                    } else {
                        target = target->getChildByName(name);
                    }
                    if (target == nullptr) {
                        const auto* importedModule = dynamic_cast<const ImportedModule*>(parentItem);
                        if (importedModule != nullptr && &part == &item.namePrefix.back() && item.nameSuffixes.empty()) {
                            const IdentifierNode* alias = item.alias.has_value() ? item.alias.value().get() : nullptr;
                            bool matched = false;
                            for (const auto& [internalName, identifier] : importedModule->getIdentifiers()) {
                                const auto* function = dynamic_cast<const ImportedFunc*>(identifier.get());
                                const auto* method = dynamic_cast<const ImportedMethod*>(identifier.get());
                                const std::string_view originalName = function != nullptr ? function->getOriginalName() : method != nullptr ? method->getOriginalName() : std::string_view();
                                if (originalName != name) {
                                    continue;
                                }

                                matched = true;
                                auto functionPath = path;
                                functionPath.push_back(internalName);
                                bind(identifier.get(), std::move(functionPath), alias, alias ? *alias : location);
                            }
                            if (matched) {
                                return;
                            }
                        }
                        context.reportError(*part, fmt::format("Could not find imported package, module or identifier '{}'", name));
                        return;
                    }
                    path.emplace_back(name);
                }
            }

            if (target == nullptr) {
                context.reportError(location, "Import path must name a package, module or identifier");
                return;
            }
            if (item.self && !dynamic_cast<const ImportedPackage*>(target) && !dynamic_cast<const ImportedModule*>(target)) {
                context.reportError(location, "Self imports can only refer to packages or modules");
                return;
            }
            if (item.wildcard) {
                const auto* importedModule = dynamic_cast<const ImportedModule*>(target);
                if (importedModule == nullptr) {
                    context.reportError(location, "Wildcard imports can only be used for modules");
                    return;
                }
                if (item.alias.has_value()) {
                    context.reportError(*item.alias.value(), "Wildcard imports cannot have aliases");
                    return;
                }
                std::vector<std::string> identifierNames;
                for (const auto& [name, identifier] : importedModule->getIdentifiers()) {
                    identifierNames.push_back(name);
                }
                std::sort(identifierNames.begin(), identifierNames.end());
                for (const auto& name : identifierNames) {
                    path.push_back(name);
                    bind(importedModule->getIdentifierByName(name), path, nullptr, location);
                    path.pop_back();
                }
            } else if (!item.nameSuffixes.empty()) {
                for (const auto& suffix : item.nameSuffixes) {
                    checkItem(*suffix, target, path);
                }
            } else {
                const auto* alias = item.alias.has_value() ? item.alias.value().get() : nullptr;
                bind(target, path, alias, alias ? *alias : location);
            }
        };

        checkItem(importItem, nullptr, {});
        if (context.getErrors().size() != errorCount) {
            return;
        }

        std::unordered_set<const ImportedPackage*> scopedPackages;
        std::unordered_set<std::string> reportedRedeclarations;

        for (const auto& binding : bindings) {
            const auto* package = getImportedRootPackageByName(binding.path.front());
            const ImportedItem* target = ImportedItem::getImportedItemByFullPath(imports, binding.path);
            if (target == nullptr) {
                continue;
            }

            bool declared = false;
            if (const auto* function = dynamic_cast<const ImportedFunc*>(target)) {
                FunctionSymbol functionSymbol(binding.name);
                functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::FUNCTION, RegularSymbolAccessModifier::PUBLIC, function->getName(), function));
                declared = context.currentScope().declare(std::move(functionSymbol));
            } else if (const auto* method = dynamic_cast<const ImportedMethod*>(target)) {
                FunctionSymbol functionSymbol(binding.name);
                functionSymbol.addOverloading(RegularSymbol(RegularSymbolKind::METHOD, getImportedAccessModifier(*method), method->getName(), method));
                declared = context.currentScope().declare(std::move(functionSymbol));
            } else {
                declared = context.currentScope().declare(RegularSymbol(getImportedSymbolKind(*target), RegularSymbolAccessModifier::PUBLIC, binding.name, target));
            }
            if (!declared && reportedRedeclarations.insert(binding.name).second) {
                context.reportError(importDecl, fmt::format("Redeclaration of symbol '{}'", binding.name));
            }
            if (declared) {
                importedRoots.push_back(target);
            }
            if (getImportedScopeKind(*target).has_value()) {
                scopedPackages.insert(package);
            }
        }

        for (const auto* package : scopedPackages) {
            registerImportedScopes(*package, nullptr);
        }

        for (const auto* root : importedRoots) {
            registerImportedTypeReferences(*root);
        }
    }

    void SemanticAnalyzer::checkExport(const ExportDeclarationNode& exportDecl) {
        for (auto& item : exportDecl.getNameList()) {
            if (context.currentScope().lookup(item.name->getIdentifierString()) == nullptr) {
                context.reportError(exportDecl, fmt::format("Undefined symbol {}", item.name->getIdentifierString()));
            }
        }
    }

    void SemanticAnalyzer::checkValueDeclaration(const ValueDeclarationNode& varDecl, MetadataInfo metadataInfo) {
        auto kind = varDecl.getKind();
        const bool hasInitializer = varDecl.getInitializer().has_value();
        const bool requiresInitializer = kind == ValueDeclarationKind::Kind::LET || kind == ValueDeclarationKind::Kind::STATIC_PROPERTY;

        if (requiresInitializer && !hasInitializer) {
            context.reportError(varDecl, kind == ValueDeclarationKind::Kind::STATIC_PROPERTY ? "Static properties must be initialized" : "Variables must be initialized");
        } else if (!requiresInitializer && hasInitializer) {
            context.reportError(
                varDecl,
                kind == ValueDeclarationKind::Kind::INSTANCE_PROPERTY ? "Instance properties cannot have initializers" : "Value declarations of this kind cannot have initializers"
            );
        }

        if (hasInitializer) {
            checkExpression(*varDecl.getInitializer().value());
        }

        if (varDecl.getType().has_value()) {
            checkType(*varDecl.getType()->get());
        }

        // TODO: Implement type inference
    }

    void SemanticAnalyzer::checkFunctionDeclaration(const FunctionDeclarationNode& funcDecl, MetadataInfo metadataInfo) {
        if (!context.pushExistingScope(&funcDecl)) {
            return;
        }

        for (const auto& param : funcDecl.getParameters()) {
            checkValueDeclaration(*param);
        }

        if (funcDecl.getKind() == FunctionDeclarationKind::Kind::REGULAR && funcDecl.getContext() != FunctionDeclarationKind::Context::INTERFACE) {
            if (funcDecl.getBody().has_value()) {
                checkStatement(*funcDecl.getBody().value());
            } else {
                context.reportError(funcDecl, "Regular functions must have a body");
            }
        }

        if (funcDecl.getReturnType().has_value()) {
            checkType(*funcDecl.getReturnType()->get());
        }

        // TODO: Implement return type inference

        context.popScope();
    }

    void SemanticAnalyzer::checkConstructorDeclaration(const ConstructorDeclarationNode& constructorDecl, MetadataInfo metadataInfo) {
        if (!context.pushExistingScope(&constructorDecl)) {
            return;
        }

        for (const auto& param : constructorDecl.getParameters()) {
            checkValueDeclaration(*param);
        }

        checkStatement(constructorDecl.getBody());

        context.popScope();
    }

    void SemanticAnalyzer::checkOperatorDeclaration(const OperatorDeclarationNode& operatorDecl, MetadataInfo metadataInfo) {
        if (!context.pushExistingScope(&operatorDecl)) {
            return;
        }

        for (const auto& param : operatorDecl.getParameters()) {
            checkValueDeclaration(*param);
        }

        if (operatorDecl.getContext() == OperatorDeclarationKind::Context::CLASS) {
            if (operatorDecl.getBody().has_value()) {
                checkStatement(*operatorDecl.getBody().value());
            } else {
                context.reportError(operatorDecl, "Class operators must have a body");
            }
        } else if (operatorDecl.getBody().has_value()) {
            context.reportError(operatorDecl, "Interface operators cannot have a body");
        }

        if (operatorDecl.getReturnType().has_value()) {
            checkType(*operatorDecl.getReturnType().value());
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkClassDeclaration(const ClassDeclarationNode& classDecl, const Config& config, MetadataInfo metadataInfo) {
        if (!context.pushExistingScope(&classDecl)) {
            return;
        }

        for (const auto& propertyDecl : classDecl.getPropertyDeclarations()) {
            checkValueDeclaration(*propertyDecl);
        }

        for (const auto& methodDecl : classDecl.getMethodDeclarations()) {
            checkFunctionDeclaration(*methodDecl);
        }

        for (const auto& constructorDecl : classDecl.getConstructorDeclarations()) {
            checkConstructorDeclaration(*constructorDecl);
        }

        for (const auto& operatorDecl : classDecl.getOperatorDeclarations()) {
            checkOperatorDeclaration(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkInterfaceDeclaration(const InterfaceDeclarationNode& interfaceDecl, const Config& config, MetadataInfo metadataInfo) {
        if (!context.pushExistingScope(&interfaceDecl)) {
            return;
        }

        for (const auto& member : interfaceDecl.getMethodDeclarations()) {
            if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(member.get())) {
                checkFunctionDeclaration(*funcDecl);
            } else {
                context.reportError(*member, "Invalid interface member declaration");
            }
        }

        for (const auto& operatorDecl : interfaceDecl.getOperatorDeclarations()) {
            checkOperatorDeclaration(*operatorDecl);
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkEnumDeclaration(const EnumDeclarationNode& enumDecl, const Config& config, MetadataInfo metadataInfo) {
        if (!context.pushExistingScope(&enumDecl)) {
            return;
        }

        for (const auto& member : enumDecl.getMemberDeclarations()) {
            if (!context.pushExistingScope(member.get())) {
                continue;
            }

            for (auto& associatedValue : member->getAssociatedValues()) {
                checkValueDeclaration(*associatedValue);
            }

            context.popScope();
        }

        context.popScope();
    }

    void SemanticAnalyzer::checkTypeAliasDeclaration(const TypeAliasDeclarationNode& typeAliasDecl, const Config& config, MetadataInfo metadataInfo) {
        if (!context.pushExistingScope(&typeAliasDecl)) {
            return;
        }

        checkType(typeAliasDecl.getOriginalType());

        context.popScope();
    }

    void SemanticAnalyzer::checkStatement(const StatementNode& statement) {
        if (auto* stmt = dynamic_cast<const BlockStatementNode*>(&statement)) {
            context.pushScope(std::make_unique<Scope>(ScopeKind::BLOCK, &context.currentScope(), stmt));

            for (const auto& child : stmt->getStatements()) {
                checkStatement(*child);
            }

            context.popScope();
        } else if (auto* stmt = dynamic_cast<const BreakStatementNode*>(&statement)) {
            if (!context.currentLoop()) {
                context.reportError(*stmt, "Break statement not within a loop");
            }

            const auto& label = stmt->getLabel();
            if (label.has_value()) {
                const auto* labelSymbol = dynamic_cast<const RegularSymbol*>(context.currentScope().lookup(label.value()->getIdentifierString()));
                if (labelSymbol == nullptr) {
                    context.reportError(*stmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
                } else if (labelSymbol->getKind() != RegularSymbolKind::LOOP_LABEL) {
                    context.reportError(*stmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
                }
            }
        } else if (auto* stmt = dynamic_cast<const ContinueStatementNode*>(&statement)) {
            if (!context.currentLoop()) {
                context.reportError(*stmt, "Continue statement not within a loop");
            }

            const auto& label = stmt->getLabel();
            if (label.has_value()) {
                const auto* labelSymbol = dynamic_cast<const RegularSymbol*>(context.currentScope().lookup(label.value()->getIdentifierString()));
                if (labelSymbol == nullptr) {
                    context.reportError(*stmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
                } else if (labelSymbol->getKind() != RegularSymbolKind::LOOP_LABEL) {
                    context.reportError(*stmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
                }
            }
        } else if (auto* stmt = dynamic_cast<const ExpressionStatementNode*>(&statement)) {
            checkExpression(stmt->getExpression());
        } else if (auto* stmt = dynamic_cast<const ForStatementNode*>(&statement)) {
            context.pushScope(std::make_unique<Scope>(ScopeKind::LOOP, &context.currentScope(), stmt));

            const auto& label = stmt->getLabel();
            if (label.has_value()) {
                RegularSymbol labelSymbol(RegularSymbolKind::LOOP_LABEL, RegularSymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), stmt);
                if (!context.currentScope().declare(std::move(labelSymbol))) {
                    context.reportError(*stmt, fmt::format("Redeclaration of identifier '{}'", label.value()->getIdentifierString()));
                }
            }

            const auto& loopVariable = stmt->getLoopVariable();
            RegularSymbol loopVariableSymbol(
                RegularSymbolKind::VARIABLE,
                static_cast<RegularSymbolAccessModifier>(loopVariable.getAccessModifier()),
                loopVariable.getName().getIdentifierString(),
                &loopVariable
            );
            if (!context.currentScope().declare(std::move(loopVariableSymbol))) {
                context.reportError(loopVariable, fmt::format("Redeclaration of symbol '{}'", loopVariable.getName().getIdentifierString()));
            }

            checkValueDeclaration(stmt->getLoopVariable());
            checkExpression(stmt->getIterableExpression());
            checkStatement(stmt->getBody());

            context.popScope();
        } else if (auto* stmt = dynamic_cast<const IfStatementNode*>(&statement)) {
            checkExpression(stmt->getCondition());
            checkStatement(stmt->getThenBranch());
            if (stmt->getElseBranch().has_value()) {
                checkStatement(*stmt->getElseBranch().value());
            }
        } else if (auto* stmt = dynamic_cast<const ReturnStatementNode*>(&statement)) {
            if (stmt->getReturnValue().has_value()) {
                checkExpression(*stmt->getReturnValue().value());
            }
        } else if (auto* stmt = dynamic_cast<const SwitchStatementNode*>(&statement)) {
            context.pushScope(std::make_unique<Scope>(ScopeKind::SWITCH, &context.currentScope(), stmt));

            checkExpression(stmt->getSwitchExpression());

            if (stmt->getSwitchKind() == SwitchStatementKind::LITERAL_MATCH) {
                for (const auto& item : stmt->getLiteralMatchItems()) {
                    checkStatement(*item.body);
                }
            } else if (stmt->getSwitchKind() == SwitchStatementKind::TYPE_MATCH) {
                for (const auto& item : stmt->getTypeMatchItems()) {
                    checkType(*item.type);
                    checkStatement(*item.body);
                }
            }

            if (stmt->getDefaultCaseBody().has_value()) {
                checkStatement(*stmt->getDefaultCaseBody().value());
            }

            context.popScope();
        } else if (auto* stmt = dynamic_cast<const VariableDeclarationStatementNode*>(&statement)) {
            const auto& varDecl = stmt->getVariableDeclaration();
            RegularSymbol variableSymbol(RegularSymbolKind::VARIABLE, static_cast<RegularSymbolAccessModifier>(varDecl.getAccessModifier()), varDecl.getName().getIdentifierString(), &varDecl);
            if (!context.currentScope().declare(std::move(variableSymbol))) {
                context.reportError(varDecl, fmt::format("Redeclaration of symbol '{}'", varDecl.getName().getIdentifierString()));
            }
            checkValueDeclaration(varDecl);
        } else if (auto* stmt = dynamic_cast<const WhileStatementNode*>(&statement)) {
            context.pushScope(std::make_unique<Scope>(ScopeKind::LOOP, &context.currentScope(), stmt));

            const auto& label = stmt->getLabel();
            if (label.has_value()) {
                RegularSymbol labelSymbol(RegularSymbolKind::LOOP_LABEL, RegularSymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), stmt);
                if (!context.currentScope().declare(std::move(labelSymbol))) {
                    context.reportError(*stmt, fmt::format("Redeclaration of identifier '{}'", label.value()->getIdentifierString()));
                }
            }

            checkExpression(stmt->getCondition());
            checkStatement(stmt->getBody());

            context.popScope();
        } else {
            context.reportError(statement, "Unknown statement type");
        }
    }

    void SemanticAnalyzer::checkExpression(const ExpressionNode& expression) {
        if (auto* expr = dynamic_cast<const BinaryExpressionNode*>(&expression)) {
            // TODO: Implement binary expression checking
            checkExpression(expr->getLeft());
            checkExpression(expr->getRight());
        } else if (auto* expr = dynamic_cast<const ConditionalExpressionNode*>(&expression)) {
            // TODO: Implement conditional expression checking
            checkExpression(expr->getCondition());
            checkExpression(expr->getThenExpression());
            checkExpression(expr->getElseExpression());
        } else if (auto* expr = dynamic_cast<const DictLiteralExpressionNode*>(&expression)) {
            // TODO: Implement dictionary literal expression checking
            for (const auto& entry : expr->getEntries()) {
                checkExpression(*entry.second);
            }
        } else if (auto* expr = dynamic_cast<const FunctionCallExpressionNode*>(&expression)) {
            // TODO: Implement function call expression checking
            checkExpression(expr->getCallee());
            for (const auto& argument : expr->getArguments()) {
                checkExpression(*argument);
            }
            if (expr->getContext().has_value()) {
                checkExpression(*expr->getContext().value());
            }
        } else if (auto* expr = dynamic_cast<const IdentifierLikeExpressionNode*>(&expression)) {
            // TODO: Implement identifier expression checking
        } else if (dynamic_cast<const PrimitiveTypeExpressionNode*>(&expression) != nullptr) {
        } else if (auto* expr = dynamic_cast<const ListLikeLiteralExpressionNode*>(&expression)) {
            // TODO: Implement list-like literal expression checking
            for (const auto& element : expr->getElements()) {
                checkExpression(*element);
            }
        } else if (auto* expr = dynamic_cast<const MemberAccessExpressionNode*>(&expression)) {
            // TODO: Implement member access expression checking
            checkExpression(expr->getObject());
            checkExpression(expr->getMember());
        } else if (auto* expr = dynamic_cast<const RangeExpressionNode*>(&expression)) {
            // TODO: Implement range expression checking
            if (expr->getStart().has_value()) {
                checkExpression(*expr->getStart().value());
            }
            if (expr->getEnd().has_value()) {
                checkExpression(*expr->getEnd().value());
            }
        } else if (auto* expr = dynamic_cast<const SelectorLiteralExpressionNode*>(&expression)) {
            // TODO: Implement selector literal expression checking
            for (const auto& argument : expr->getArguments()) {
                checkExpression(*argument.second);
            }
        } else if (auto* expr = dynamic_cast<const SimpleLiteralExpressionNode*>(&expression)) {
            // TODO: Implement simple literal expression checking
        } else if (auto* expr = dynamic_cast<const StringLiteralExpressionNode*>(&expression)) {
            // TODO: Implement string literal expression checking
            for (const auto& part : expr->getParts()) {
                if (const auto* nestedExpression = std::get_if<std::unique_ptr<ExpressionNode>>(&part)) {
                    checkExpression(**nestedExpression);
                }
            }
        } else if (auto* expr = dynamic_cast<const SubscriptExpressionNode*>(&expression)) {
            // TODO: Implement subscript expression checking
            checkExpression(expr->getObject());
            checkExpression(expr->getIndex());
        } else if (auto* expr = dynamic_cast<const SuperExpressionNode*>(&expression)) {
            // TODO: Implement super expression checking
        } else if (auto* expr = dynamic_cast<const NoneExpressionNode*>(&expression)) {
            // TODO: Implement none expression checking
        } else if (auto* expr = dynamic_cast<const ThisExpressionNode*>(&expression)) {
            // TODO: Implement this expression checking
        } else if (auto* expr = dynamic_cast<const UnaryExpressionNode*>(&expression)) {
            // TODO: Implement unary expression checking
            checkExpression(expr->getOperand());
        } else {
            context.reportError(expression, "Unknown expression type");
        }
    }

    const Type* SemanticAnalyzer::checkType(const TypeReferenceNode& type) {
        const auto getPrimitiveType = [](PrimitiveTypeReferenceKind kind) -> const PrimitiveType* {
            switch (kind) {
                case PrimitiveTypeReferenceKind::BYTE:
                    return PrimitiveType::byteType();
                case PrimitiveTypeReferenceKind::SHORT:
                    return PrimitiveType::shortType();
                case PrimitiveTypeReferenceKind::INT:
                    return PrimitiveType::intType();
                case PrimitiveTypeReferenceKind::LONG:
                    return PrimitiveType::longType();
                case PrimitiveTypeReferenceKind::FLOAT:
                    return PrimitiveType::floatType();
                case PrimitiveTypeReferenceKind::DOUBLE:
                    return PrimitiveType::doubleType();
                case PrimitiveTypeReferenceKind::BOOLEAN:
                    return PrimitiveType::booleanType();
                case PrimitiveTypeReferenceKind::STRING:
                    return PrimitiveType::stringType();
            }
            return nullptr;
        };

        const auto getGenericParameterCount = [](const RegularSymbol& symbol) -> std::size_t {
            if (symbol.getKind() == RegularSymbolKind::GENERIC_PARAMETER) {
                return 0;
            }
            if (symbol.getLocalNode() != nullptr) {
                if (const auto* classDeclaration = dynamic_cast<const ClassDeclarationNode*>(symbol.getLocalNode())) {
                    return classDeclaration->getGenericParameterNames().size();
                }
                if (const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationNode*>(symbol.getLocalNode())) {
                    return interfaceDeclaration->getGenericParameterNames().size();
                }
                if (const auto* enumDeclaration = dynamic_cast<const EnumDeclarationNode*>(symbol.getLocalNode())) {
                    return enumDeclaration->getGenericParameterNames().size();
                }
                if (const auto* typeAliasDeclaration = dynamic_cast<const TypeAliasDeclarationNode*>(symbol.getLocalNode())) {
                    return typeAliasDeclaration->getGenericParameterNames().size();
                }
            } else if (symbol.getImportedNode() != nullptr) {
                if (const auto* importedClass = dynamic_cast<const ImportedClass*>(symbol.getImportedNode())) {
                    return importedClass->getGenericParameters().size();
                }
                if (const auto* importedInterface = dynamic_cast<const ImportedInterface*>(symbol.getImportedNode())) {
                    return importedInterface->getGenericParameters().size();
                }
                if (const auto* importedEnum = dynamic_cast<const ImportedEnum*>(symbol.getImportedNode())) {
                    return importedEnum->getGenericParameters().size();
                }
                if (const auto* importedTypeAlias = dynamic_cast<const ImportedTypeAlias*>(symbol.getImportedNode())) {
                    return importedTypeAlias->getGenericParameters().size();
                }
            }
            return 0;
        };

        using TypeSubstitutionMap = std::unordered_map<std::string, const Type*>;

        using NameSubstitutionMap = std::unordered_map<std::string, std::string>;

        const auto substituteImportedTypeParametersWithNames =
            [](std::string_view fullTypeName, const std::vector<std::string>& parameterNames, const std::vector<std::string>& argumentNames) {
                std::string result;
                std::size_t tokenStart = 0;
                const auto appendToken = [&](std::string_view token) {
                    if (token.empty()) {
                        return;
                    }

                    for (std::size_t index = 0; index < parameterNames.size() && index < argumentNames.size(); ++index) {
                        if (token == parameterNames[index]) {
                            result += argumentNames[index];
                            return;
                        }
                    }
                    result += token;
                };

                for (std::size_t index = 0; index < fullTypeName.size(); ++index) {
                    const char character = fullTypeName[index];
                    if (character != '<' && character != '>' && character != ',' && character != ' ' && character != '\t' && character != '\r' && character != '\n') {
                        continue;
                    }

                    appendToken(fullTypeName.substr(tokenStart, index - tokenStart));
                    if (character == '<' || character == '>') {
                        result += character;
                    } else if (character == ',') {
                        result += ", ";
                    }
                    tokenStart = index + 1;
                }

                appendToken(fullTypeName.substr(tokenStart));
                return result;
            };

        std::function<std::string(const TypeReferenceNode&, const NameSubstitutionMap&)> getFullTypeNameByTypeReferenceNode;

        const std::function<std::string(const TypeReferenceNode&, const NameSubstitutionMap&)> getUnwrappedTypeNameByTypeReferenceNode =
            [&](const TypeReferenceNode& typeNode, const NameSubstitutionMap& substitutions) -> std::string {
            if (const auto* primitiveTypeReferenceNode = dynamic_cast<const PrimitiveTypeReferenceNode*>(&typeNode)) {
                return std::string(getPrimitiveType(primitiveTypeReferenceNode->getKind())->getFullTypeName());
            }

            const auto* customizedTypeReferenceNode = dynamic_cast<const CustomizedTypeReferenceNode*>(&typeNode);
            if (customizedTypeReferenceNode == nullptr) {
                return "";
            }

            const auto& nameParts = customizedTypeReferenceNode->getNameParts();
            if (nameParts.size() == 1) {
                const auto substitution = substitutions.find(std::string(nameParts.front()->getIdentifierString()));
                if (substitution != substitutions.end()) {
                    return substitution->second;
                }
            }

            std::stack<std::string> identifiers;
            std::string result;

            const Scope* currentScope = &context.currentScope();
            while (currentScope && !currentScope->lookupLocal(nameParts.front()->getIdentifierString())) {
                currentScope = currentScope->findParent();
            }

            if (!currentScope) {
                context.reportError(*nameParts.front(), fmt::format("Use of undeclared identifier {}", nameParts.front()->getIdentifierString()));
                return "";
            }

            const auto* firstSymbol = dynamic_cast<const RegularSymbol*>(currentScope->lookupLocal(nameParts.front()->getIdentifierString()));
            if (firstSymbol == nullptr) {
                context.reportError(*nameParts.front(), fmt::format("Use of undeclared identifier {}", nameParts.front()->getIdentifierString()));
                return "";
            }

            const RegularSymbol* referencedSymbol = firstSymbol;
            if (nameParts.size() > 1) {
                const Scope* referencedScope = context.getScopeByRegularSymbol(*firstSymbol);
                for (std::size_t index = 1; index < nameParts.size(); ++index) {
                    if (referencedScope == nullptr) {
                        return "";
                    }

                    const auto* symbol = dynamic_cast<const RegularSymbol*>(referencedScope->lookup(nameParts[index]->getIdentifierString()));
                    if (symbol == nullptr) {
                        return "";
                    }

                    referencedSymbol = symbol;
                    if (index + 1 < nameParts.size()) {
                        referencedScope = context.getScopeByRegularSymbol(*symbol);
                    }
                }
            }

            if (firstSymbol->getKind() == RegularSymbolKind::GENERIC_PARAMETER) {
                result = std::string(firstSymbol->getName());
            } else if (referencedSymbol->getKind() == RegularSymbolKind::TYPE_ALIAS) {
                const auto& genericArguments = customizedTypeReferenceNode->getGenericArguments();

                if (referencedSymbol->getOrigin() == RegularSymbolOrigin::LOCAL) {
                    const auto* typeAliasDeclaration = dynamic_cast<const TypeAliasDeclarationNode*>(referencedSymbol->getLocalNode());
                    if (typeAliasDeclaration == nullptr) {
                        return "";
                    }

                    NameSubstitutionMap aliasSubstitutions;
                    const auto& genericParameterNames = typeAliasDeclaration->getGenericParameterNames();
                    for (std::size_t index = 0; index < genericParameterNames.size() && index < genericArguments.size(); ++index) {
                        aliasSubstitutions.emplace(
                            std::string(genericParameterNames[index]->getIdentifierString()),
                            getFullTypeNameByTypeReferenceNode(*genericArguments[index], substitutions)
                        );
                    }
                    return getFullTypeNameByTypeReferenceNode(typeAliasDeclaration->getOriginalType(), aliasSubstitutions);
                }

                if (referencedSymbol->getOrigin() == RegularSymbolOrigin::IMPORTED) {
                    const auto* importedTypeAlias = dynamic_cast<const ImportedTypeAlias*>(referencedSymbol->getImportedNode());
                    if (importedTypeAlias == nullptr) {
                        return "";
                    }

                    std::vector<std::string> argumentNames;
                    for (const auto& genericArgument : genericArguments) {
                        argumentNames.emplace_back(getFullTypeNameByTypeReferenceNode(*genericArgument, substitutions));
                    }
                    return substituteImportedTypeParametersWithNames(importedTypeAlias->getOriginalType(), importedTypeAlias->getGenericParameters(), argumentNames);
                }
            } else if (firstSymbol->getOrigin() == RegularSymbolOrigin::LOCAL) {
                const Scope* localScope = context.getScopeByRegularSymbol(*firstSymbol);
                if (localScope != nullptr) {
                    while (localScope->findParent() && localScope->getKind() != ScopeKind::MODULE) {
                        const TypeDeclarationNode* node = dynamic_cast<const TypeDeclarationNode*>(localScope->getLocalNode());
                        if (const ClassDeclarationNode* classDecl = dynamic_cast<const ClassDeclarationNode*>(node)) {
                            identifiers.push(std::string(classDecl->getName().getIdentifierString()));
                        } else if (const InterfaceDeclarationNode* interfaceDecl = dynamic_cast<const InterfaceDeclarationNode*>(node)) {
                            identifiers.push(std::string(interfaceDecl->getName().getIdentifierString()));
                        } else if (const EnumDeclarationNode* enumDecl = dynamic_cast<const EnumDeclarationNode*>(node)) {
                            identifiers.push(std::string(enumDecl->getName().getIdentifierString()));
                        } else if (const EnumMemberDeclarationNode* enumMemberDecl = dynamic_cast<const EnumMemberDeclarationNode*>(node)) {
                            identifiers.push(std::string(enumMemberDecl->getName().getIdentifierString()));
                        } else if (const TypeAliasDeclarationNode* typeAliasDecl = dynamic_cast<const TypeAliasDeclarationNode*>(node)) {
                            identifiers.push(std::string(typeAliasDecl->getAliasName().getIdentifierString()));
                        }

                        localScope = localScope->findParent();
                    }
                }

                result = module.getFullName();
                while (!identifiers.empty()) {
                    result.append(".");
                    result.append(identifiers.top());
                    identifiers.pop();
                }
                const std::size_t firstIndex = localScope == nullptr ? 0 : 1;
                for (std::size_t index = firstIndex; index < nameParts.size(); ++index) {
                    result.append(".");
                    result.append(nameParts[index]->getIdentifierString());
                }
            } else if (firstSymbol->getOrigin() == RegularSymbolOrigin::IMPORTED) {
                const Scope* importedScope = context.getScopeByRegularSymbol(*firstSymbol);
                if (importedScope == nullptr) {
                    context.reportError(*nameParts.front(), fmt::format("Identifier '{}' does not have a scope", nameParts.front()->getIdentifierString()));
                    return "";
                }
                while (importedScope != nullptr) {
                    if (const ImportedItem* importedNode = importedScope->getImportedNode()) {
                        identifiers.push(std::string(importedNode->getName()));
                    }
                    importedScope = importedScope->findParent();
                }
                if (identifiers.empty()) {
                    context.reportError(*nameParts.front(), fmt::format("Identifier '{}' does not have a scope", nameParts.front()->getIdentifierString()));
                    return "";
                }
                result = std::move(identifiers.top());
                identifiers.pop();
                while (!identifiers.empty()) {
                    result.append(".");
                    result.append(identifiers.top());
                    identifiers.pop();
                }
                for (std::size_t index = 1; index < nameParts.size(); ++index) {
                    result.append(".");
                    result.append(nameParts[index]->getIdentifierString());
                }
            }

            const auto& genericArguments = customizedTypeReferenceNode->getGenericArguments();
            if (!genericArguments.empty()) {
                std::string argumentList = "<";
                for (const auto& genericArgument : genericArguments) {
                    argumentList += getFullTypeNameByTypeReferenceNode(*genericArgument, substitutions);
                    argumentList += ", ";
                }
                argumentList.pop_back();
                argumentList.pop_back();
                argumentList += '>';
                result += argumentList;
            }

            return result;
        };

        getFullTypeNameByTypeReferenceNode = [&](const TypeReferenceNode& typeNode, const NameSubstitutionMap& substitutions) {
            std::string result = getUnwrappedTypeNameByTypeReferenceNode(typeNode, substitutions);
            if (!result.empty() && typeNode.hasQuestionMarkSuffix()) {
                return fmt::format("vanillang.typesystem.Optional<{}>", result);
            }
            return result;
        };

        const std::function<const Type*(const TypeReferenceNode&, const TypeSubstitutionMap&, std::unordered_set<const TypeReferenceNode*>)> checkTypeWithCyclicDetection =
            [&](const TypeReferenceNode& type, const TypeSubstitutionMap& substitutions, std::unordered_set<const TypeReferenceNode*> visited) -> const Type* {
            if (!visited.insert(&type).second) {
                context.reportError(type, "Cyclic type reference detected");
                return nullptr;
            }

            if (substitutions.empty()) {
                const Type* existedType = context.getTypeByTypeReferenceNode(&type);
                if (existedType) {
                    return existedType; // avoid resolving repeatitively
                }
            }

            const auto checkUnwrappedType = [&](const TypeReferenceNode& type) -> const Type* {
                if (const auto* primitiveTypeReferenceNode = dynamic_cast<const PrimitiveTypeReferenceNode*>(&type)) {
                    return getPrimitiveType(primitiveTypeReferenceNode->getKind());
                }

                const auto* customizedTypeReferenceNode = dynamic_cast<const CustomizedTypeReferenceNode*>(&type);
                if (customizedTypeReferenceNode == nullptr) {
                    return nullptr;
                }

                const auto& nameParts = customizedTypeReferenceNode->getNameParts();
                if (nameParts.size() == 1) {
                    const auto substitution = substitutions.find(std::string(nameParts.front()->getIdentifierString()));
                    if (substitution != substitutions.end()) {
                        return substitution->second;
                    }
                }
                const auto isTypeDefinition = [](RegularSymbolKind kind) {
                    switch (kind) {
                        case RegularSymbolKind::CLASS:
                        case RegularSymbolKind::INTERFACE:
                        case RegularSymbolKind::ENUM:
                        case RegularSymbolKind::ENUM_MEMBER:
                        case RegularSymbolKind::TYPE_ALIAS:
                        case RegularSymbolKind::GENERIC_PARAMETER:
                            return true;
                        default:
                            return false;
                    }
                };

                bool hasResolvedType = false;
                const RegularSymbol* typeSymbol = nullptr;

                const Scope* scope = &context.currentScope();
                for (std::size_t index = 0; index < nameParts.size(); ++index) {
                    const auto& namePart = nameParts[index];
                    const auto* symbol = dynamic_cast<const RegularSymbol*>(scope->lookup(namePart->getIdentifierString()));
                    if (symbol == nullptr) {
                        context.reportError(*namePart, fmt::format("Use of undeclared type '{}'", namePart->getIdentifierString()));
                        break;
                    }

                    if (index == nameParts.size() - 1) {
                        if (!isTypeDefinition(symbol->getKind())) {
                            context.reportError(*namePart, fmt::format("Identifier '{}' is not a type definition", namePart->getIdentifierString()));
                        } else {
                            typeSymbol = symbol;
                            hasResolvedType = true;
                        }
                        break;
                    }

                    scope = context.getScopeByRegularSymbol(*symbol);
                    if (scope == nullptr) {
                        context.reportError(*namePart, fmt::format("Identifier '{}' does not have a scope", namePart->getIdentifierString()));
                        break;
                    }
                }

                if (!hasResolvedType) {
                    return nullptr;
                }

                const std::size_t genericArgumentCount = customizedTypeReferenceNode->getGenericArguments().size();
                const std::size_t genericParameterCount = typeSymbol == nullptr ? 0 : getGenericParameterCount(*typeSymbol);
                if (genericArgumentCount != genericParameterCount) {
                    context.reportError(type, fmt::format("Generic argument count mismatch: expected {}, got {}", genericParameterCount, genericArgumentCount));
                    return nullptr;
                }

                std::vector<const Type*> genericArgumentTypes;
                std::size_t errors = context.getErrors().size();
                for (const auto& genericArgument : customizedTypeReferenceNode->getGenericArguments()) {
                    const Type* genericArgumentType = checkTypeWithCyclicDetection(*genericArgument, substitutions, visited);
                    if (genericArgumentType == nullptr) {
                        return nullptr;
                    }
                    genericArgumentTypes.emplace_back(genericArgumentType);
                }
                if (context.getErrors().size() > errors) {
                    return nullptr;
                }

                if (typeSymbol->getKind() == RegularSymbolKind::TYPE_ALIAS) {
                    if (typeSymbol->getOrigin() == RegularSymbolOrigin::LOCAL) {
                        const auto* typeAliasDeclaration = dynamic_cast<const TypeAliasDeclarationNode*>(typeSymbol->getLocalNode());
                        if (typeAliasDeclaration == nullptr) {
                            return nullptr;
                        }

                        TypeSubstitutionMap aliasSubstitutions;
                        const auto& genericParameterNames = typeAliasDeclaration->getGenericParameterNames();
                        for (std::size_t index = 0; index < genericParameterNames.size(); ++index) {
                            aliasSubstitutions.emplace(std::string(genericParameterNames[index]->getIdentifierString()), genericArgumentTypes[index]);
                        }

                        return checkTypeWithCyclicDetection(typeAliasDeclaration->getOriginalType(), aliasSubstitutions, visited);
                    }

                    if (typeSymbol->getOrigin() == RegularSymbolOrigin::IMPORTED) {
                        const auto substituteImportedTypeParameters =
                            [](std::string_view fullTypeName, const std::vector<std::string>& parameterNames, const std::vector<const Type*>& arguments) {
                                std::string result;
                                std::size_t tokenStart = 0;
                                const auto appendToken = [&](std::string_view token) {
                                    if (token.empty()) {
                                        return;
                                    }

                                    for (std::size_t index = 0; index < parameterNames.size() && index < arguments.size(); ++index) {
                                        if (token == parameterNames[index]) {
                                            if (arguments[index] != nullptr) {
                                                result += arguments[index]->getFullTypeName();
                                            }
                                            return;
                                        }
                                    }
                                    result += token;
                                };

                                for (std::size_t index = 0; index < fullTypeName.size(); ++index) {
                                    const char character = fullTypeName[index];
                                    if (character != '<' && character != '>' && character != ',' && character != ' ' && character != '\t' && character != '\r' && character != '\n') {
                                        continue;
                                    }

                                    appendToken(fullTypeName.substr(tokenStart, index - tokenStart));
                                    if (character == '<' || character == '>') {
                                        result += character;
                                    } else if (character == ',') {
                                        result += ", ";
                                    }
                                    tokenStart = index + 1;
                                }

                                appendToken(fullTypeName.substr(tokenStart));
                                return result;
                            };

                        const auto* importedTypeAlias = dynamic_cast<const ImportedTypeAlias*>(typeSymbol->getImportedNode());
                        if (importedTypeAlias == nullptr) {
                            return nullptr;
                        }

                        const std::string originalTypeName =
                            substituteImportedTypeParameters(importedTypeAlias->getOriginalType(), importedTypeAlias->getGenericParameters(), genericArgumentTypes);
                        return registerImportedTypeReference(originalTypeName);
                    }

                    return nullptr;
                }

                NameSubstitutionMap nameSubstitutions;
                for (const auto& [name, substitutedType] : substitutions) {
                    if (substitutedType != nullptr) {
                        nameSubstitutions.emplace(name, substitutedType->getFullTypeName());
                    }
                }
                const std::string fullName = getUnwrappedTypeNameByTypeReferenceNode(type, nameSubstitutions);

                const auto customizedKind = [&typeSymbol]() -> std::optional<CustomizedTypeKind> {
                    switch (typeSymbol->getKind()) {
                        case RegularSymbolKind::CLASS:
                            return CustomizedTypeKind::CLASS;
                        case RegularSymbolKind::INTERFACE:
                            return CustomizedTypeKind::INTERFACE;
                        case RegularSymbolKind::ENUM:
                            return CustomizedTypeKind::ENUM;
                        case RegularSymbolKind::ENUM_MEMBER:
                            return CustomizedTypeKind::ENUM_MEMBER;
                        case RegularSymbolKind::GENERIC_PARAMETER:
                            return CustomizedTypeKind::GENERIC_PARAMETER;
                        default:
                            return std::nullopt;
                    }
                }();
                if (!customizedKind.has_value()) {
                    return nullptr;
                }

                const CustomizedType* existingCustomizedType = context.getCustomizedTypeByFullTypeName(fullName);
                if (existingCustomizedType != nullptr) {
                    return existingCustomizedType;
                }

                std::unique_ptr<CustomizedType> customizedType;
                if (const auto* localNode = dynamic_cast<const TypeDeclarationNode*>(typeSymbol->getLocalNode())) {
                    customizedType = std::make_unique<CustomizedType>(customizedKind.value(), fullName, std::move(genericArgumentTypes), localNode);
                } else if (const auto* importedNode = dynamic_cast<const ImportedIdentifier*>(typeSymbol->getImportedNode())) {
                    customizedType = std::make_unique<CustomizedType>(customizedKind.value(), fullName, std::move(genericArgumentTypes), importedNode);
                } else {
                    return nullptr;
                }

                const Type* resolvedType = customizedType.get();
                context.registerCustomizedType(std::move(customizedType));
                return resolvedType;
            };

            const Type* resolvedType = checkUnwrappedType(type);
            if (resolvedType == nullptr) {
                return nullptr;
            }

            if (type.hasQuestionMarkSuffix()) {
                const std::string fullName = fmt::format("vanillang.typesystem.Optional<{}>", resolvedType->getFullTypeName());
                const CustomizedType* optionalType = context.getCustomizedTypeByFullTypeName(fullName);

                if (optionalType == nullptr) {
                    std::unique_ptr<CustomizedType> customizedType = nullptr;
                    const auto* currModule = dynamic_cast<const ModuleNode*>(context.currentModule()->getLocalNode());

                    if (currModule->getFullName() == "vanillang.typesystem") {
                        const EnumDeclarationNode* opt = nullptr;
                        for (const auto& decl : currModule->getTopIdentifierDeclarations()) {
                            const EnumDeclarationNode* enumDecl = dynamic_cast<const EnumDeclarationNode*>(decl.get());

                            if (enumDecl && enumDecl->getName().getIdentifierString() == "Optional") {
                                opt = enumDecl;
                                break;
                            }
                        }

                        if (!opt) {
                            context.reportError(type, "Could not find the declaration of 'vanillang.typesystem.Optional'");
                            return nullptr;
                        }

                        customizedType = std::make_unique<CustomizedType>(CustomizedTypeKind::ENUM, fullName, std::vector<const Type*>{ resolvedType }, opt);
                    } else {
                        const ImportedItem* item = ImportedItem::getImportedItemByFullPath(imports, { "vanillang", "typesystem", "Optional" });
                        const ImportedEnum* opt = dynamic_cast<const ImportedEnum*>(item);

                        if (!opt) {
                            context.reportError(type, "Could not find the declaration of 'vanillang.typesystem.Optional'");
                            return nullptr;
                        }

                        customizedType = std::make_unique<CustomizedType>(CustomizedTypeKind::ENUM, fullName, std::vector<const Type*>{ resolvedType }, opt);
                    }

                    optionalType = customizedType.get();
                    context.registerCustomizedType(std::move(customizedType));
                }

                resolvedType = optionalType;
            }

            if (substitutions.empty()) {
                context.mapType(&type, resolvedType);
            }
            return resolvedType;
        };

        return checkTypeWithCyclicDetection(type, TypeSubstitutionMap{}, {});
    }

    TypeInferenceResult SemanticAnalyzer::inferExpressionType(const ExpressionNode& expression) {
        // TODO: Implement expression type inference

        return TypeInferenceResult::failed();
    }

    TypeInferenceResult SemanticAnalyzer::inferReturnType(const BlockStatementNode& statement) {
        // TODO: Implement return type inference

        return TypeInferenceResult::failed();
    }

    SemanticResult SemanticAnalyzer::analyze(const Config& config) {
        checkModule(module, config);

        auto diagnostics = context.takeDiagnostics();
        auto customizedTypes = context.takeCustomizedTypeRegistry();
        auto scopes = context.takeLocalScopeMap();
        auto importedScopes = context.takeImportedScopeMap();
        auto types = context.takeTypeMap();
        auto inferredValueTypes = context.takeInferredValueTypeMap();
        auto inferredFunctionReturnTypes = context.takeInferredFunctionReturnTypeMap();
        auto inferredOperatorReturnTypes = context.takeInferredOperatorReturnTypeMap();
        auto inferredExpressionTypes = context.takeInferredExpressionTypeMap();
        return SemanticResult(
            std::move(std::get<0>(diagnostics)),
            std::move(std::get<1>(diagnostics)),
            std::move(std::get<2>(diagnostics)),
            std::move(customizedTypes),
            std::move(scopes),
            std::move(importedScopes),
            std::move(types),
            std::move(inferredValueTypes),
            std::move(inferredFunctionReturnTypes),
            std::move(inferredOperatorReturnTypes),
            std::move(inferredExpressionTypes)
        );
    }
} // namespace vnlc
