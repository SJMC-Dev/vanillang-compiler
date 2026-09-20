#include "SemanticAnalyzer.hpp"
#include "ast/expression/IdentifierExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionNode.hpp"
#include "ast/expression/SuperExpressionNode.hpp"
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
#include "error/ModuleInterfaceReaderError.hpp"
#include "error/PackageReaderError.hpp"
#include "symbol/SymbolAccessModifier.hpp"
#include "symbol/SymbolKind.hpp"
#include "type/CustomizedType.hpp"
#include "type/CustomizedTypeOrigin.hpp"
#include "type/SemanticType.hpp"
#include "type/TypeExpressionType.hpp"
#include "type/typeinf/TypeInferenceResult.hpp"
#include "vni/import/ImportedAlias.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedEnumValue.hpp"
#include "vni/import/ImportedFunc.hpp"
#include "vni/import/ImportedInterface.hpp"
#include "vni/import/ImportedLet.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedParameter.hpp"
#include "vni/import/ImportedProperty.hpp"
#include "vni/import/ImportedTypeAlias.hpp"
#include "vni/import/PackageReader.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>

namespace vnlc {
    SemanticAnalyzer::SemanticAnalyzer(const ModuleNode& module) : module(module) {}

    SymbolKind SemanticAnalyzer::getImportedSymbolKind(const ImportedItem& item) {
        if (dynamic_cast<const ImportedPackage*>(&item)) return SymbolKind::PACKAGE;
        if (dynamic_cast<const ImportedModule*>(&item)) return SymbolKind::MODULE;
        if (dynamic_cast<const ImportedLet*>(&item)) return SymbolKind::VARIABLE;
        if (dynamic_cast<const ImportedFunc*>(&item)) return SymbolKind::FUNCTION;
        if (dynamic_cast<const ImportedClass*>(&item)) return SymbolKind::CLASS;
        if (dynamic_cast<const ImportedInterface*>(&item)) return SymbolKind::INTERFACE;
        if (dynamic_cast<const ImportedEnum*>(&item)) return SymbolKind::ENUM;
        if (dynamic_cast<const ImportedTypeAlias*>(&item)) return SymbolKind::TYPE_ALIAS;
        if (dynamic_cast<const ImportedEnumMember*>(&item)) return SymbolKind::ENUM_MEMBER;
        if (dynamic_cast<const ImportedProperty*>(&item) || dynamic_cast<const ImportedEnumValue*>(&item)) return SymbolKind::PROPERTY;
        if (dynamic_cast<const ImportedMethod*>(&item)) return SymbolKind::METHOD;
        if (dynamic_cast<const ImportedParameter*>(&item)) return SymbolKind::PARAMETER;
        return SymbolKind::IMPORT_ALIAS;
    }

    std::optional<ScopeKind> SemanticAnalyzer::getImportedScopeKind(const ImportedItem& item) {
        switch (getImportedSymbolKind(item)) {
            case SymbolKind::PACKAGE:
                return ScopeKind::PACKAGE;
            case SymbolKind::MODULE:
                return ScopeKind::MODULE;
            case SymbolKind::CLASS:
                return ScopeKind::CLASS;
            case SymbolKind::INTERFACE:
                return ScopeKind::INTERFACE;
            case SymbolKind::ENUM:
                return ScopeKind::ENUM;
            case SymbolKind::ENUM_MEMBER:
                return ScopeKind::ENUM_MEMBER;
            case SymbolKind::TYPE_ALIAS:
                return ScopeKind::TYPE_ALIAS;
            case SymbolKind::FUNCTION:
            case SymbolKind::METHOD:
                return ScopeKind::FUNCTION;
            default:
                return std::nullopt;
        }
    }

    SymbolAccessModifier SemanticAnalyzer::getImportedAccessModifier(const ImportedItem& item) {
        std::string_view accessModifier;
        if (const auto* property = dynamic_cast<const ImportedProperty*>(&item)) {
            accessModifier = property->getAccessModifier();
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&item)) {
            accessModifier = method->getAccessModifier();
        }
        if (accessModifier == "private") return SymbolAccessModifier::PRIVATE;
        if (accessModifier == "protected") return SymbolAccessModifier::PROTECTED;
        return SymbolAccessModifier::PUBLIC;
    }

    void SemanticAnalyzer::collectTypeDependencies(std::string_view type, std::unordered_set<std::string>& dependencies) {
        constexpr std::string_view delimiters = "<>,? \t\r\n";
        auto begin = type.find_first_not_of(delimiters);
        while (begin != std::string_view::npos) {
            auto end = type.find_first_of(delimiters, begin);
            const auto name = type.substr(begin, end == std::string_view::npos ? type.size() - begin : end - begin);
            if (name.find('.') != std::string_view::npos) {
                dependencies.emplace(name);
            }
            begin = end == std::string_view::npos ? end : type.find_first_not_of(delimiters, end);
        }
    }

    void SemanticAnalyzer::collectImportDependencies(const ImportedItem& item, std::unordered_set<std::string>& dependencies, std::unordered_set<const ImportedModule*>& visitedModules) {
        const auto collectChildren = [&](const auto& children) {
            for (const auto& [name, child] : children) {
                collectImportDependencies(*child, dependencies, visitedModules);
            }
        };

        if (const auto* package = dynamic_cast<const ImportedPackage*>(&item)) {
            collectChildren(package->getSubPackages());
            collectChildren(package->getModules());
        } else if (const auto* module = dynamic_cast<const ImportedModule*>(&item)) {
            if (visitedModules.insert(module).second) {
                collectChildren(module->getIdentifiers());
            }
        } else if (const auto* alias = dynamic_cast<const ImportedAlias*>(&item)) {
            dependencies.emplace(alias->getSource());
        } else if (const auto* variable = dynamic_cast<const ImportedLet*>(&item)) {
            collectTypeDependencies(variable->getType(), dependencies);
        } else if (const auto* property = dynamic_cast<const ImportedProperty*>(&item)) {
            collectTypeDependencies(property->getType(), dependencies);
        } else if (const auto* parameter = dynamic_cast<const ImportedParameter*>(&item)) {
            collectTypeDependencies(parameter->getType(), dependencies);
        } else if (const auto* enumValue = dynamic_cast<const ImportedEnumValue*>(&item)) {
            collectTypeDependencies(enumValue->getType(), dependencies);
        } else if (const auto* function = dynamic_cast<const ImportedFunc*>(&item)) {
            collectTypeDependencies(function->getReturnType(), dependencies);
            collectChildren(function->getParameters());
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&item)) {
            collectTypeDependencies(method->getReturnType(), dependencies);
            collectChildren(method->getParameters());
        } else if (const auto* classType = dynamic_cast<const ImportedClass*>(&item)) {
            if (classType->getBaseClass().has_value()) {
                collectTypeDependencies(classType->getBaseClass().value(), dependencies);
            }
            for (const auto& interfaceType : classType->getImplementedInterfaces()) {
                collectTypeDependencies(interfaceType, dependencies);
            }
            collectChildren(classType->getProperties());
            collectChildren(classType->getMethods());
        } else if (const auto* interfaceType = dynamic_cast<const ImportedInterface*>(&item)) {
            collectChildren(interfaceType->getMethods());
        } else if (const auto* enumType = dynamic_cast<const ImportedEnum*>(&item)) {
            collectChildren(enumType->getMembers());
        } else if (const auto* enumMember = dynamic_cast<const ImportedEnumMember*>(&item)) {
            collectChildren(enumMember->getAssociatedValues());
        } else if (const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(&item)) {
            collectTypeDependencies(typeAlias->getOriginalType(), dependencies);
        }
    }

    void SemanticAnalyzer::registerImportedScopes(const ImportedItem& item, const Scope* parent) {
        const auto kind = getImportedScopeKind(item);
        if (!kind.has_value()) return;
        auto& scope = context.getOrCreateImportedScope(kind.value(), parent, item);
        const auto declareChildren = [&](const auto& children) {
            for (const auto& [name, child] : children) {
                scope.declare(Symbol(getImportedSymbolKind(*child), getImportedAccessModifier(*child), name, child.get()));
                registerImportedScopes(*child, &scope);
            }
        };
        const auto declareGenericParameters = [&](const auto& parameters) {
            for (const auto& name : parameters) {
                scope.declare(Symbol(SymbolKind::GENERIC_PARAMETER, SymbolAccessModifier::PUBLIC, name, &item));
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
            declareGenericParameters(classType->getGenericParameters());
        } else if (const auto* interfaceType = dynamic_cast<const ImportedInterface*>(&item)) {
            declareChildren(interfaceType->getMethods());
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
        }
    }

    void SemanticAnalyzer::checkIdentifierExpressionUse(const IdentifierExpressionNode& exprNode, MetadataInfo metadataInfo) {
        const Symbol* symbol = context.currentScope().lookup(exprNode.getName().getIdentifierString());
        if (symbol == nullptr) {
            context.reportError(exprNode, fmt::format("Use of undeclared identifier '{}'", exprNode.getName().getIdentifierString()));
        } else if (!(dynamic_cast<const ValueDeclarationNode*>(symbol->getLocalNode()) || dynamic_cast<const FunctionDeclarationNode*>(symbol->getLocalNode()))) {
            context.reportError(exprNode, fmt::format("Identifier '{}' is not a variable or function", exprNode.getName().getIdentifierString()));
        }
    }

    bool SemanticAnalyzer::checkAccessModifier(const MemberAccessExpressionNode& memberAccessNode) {
        const auto& prefix = memberAccessNode.getObject();
        const auto& member = memberAccessNode.getMember();

        const SemanticType* prefixType = context.getInferredExpressionType(&prefix);
        if (const auto* typeExpression = dynamic_cast<const TypeExpressionType*>(prefixType)) {
            prefixType = typeExpression->getExpressedType();
        }

        const auto* customizedType = dynamic_cast<const CustomizedType*>(prefixType);
        if (customizedType == nullptr) {
            return true;
        }
        if (customizedType->getOrigin() == CustomizedTypeOrigin::IMPORTED) {
            // TODO: Check access to imported members.
            return true;
        }

        return checkMemberAccessModifier(customizedType->getLocalNode(), member.getName().getIdentifierString(), dynamic_cast<const SuperExpressionNode*>(&prefix) != nullptr);
    }

    bool SemanticAnalyzer::checkAccessModifier(const IdentifierExpressionNode& identifierNode) {
        const auto* currentClass = context.currentClass();
        if (currentClass == nullptr) {
            return true;
        }

        const auto memberName = identifierNode.getName().getIdentifierString();
        for (const auto* scope = &context.currentScope(); scope != nullptr && scope != currentClass; scope = scope->findParent()) {
            if (scope->lookupLocal(memberName) != nullptr) {
                return true;
            }
        }

        const Symbol* symbol = currentClass->lookupLocal(memberName);
        if (symbol != nullptr && symbol->getKind() != SymbolKind::PROPERTY && symbol->getKind() != SymbolKind::METHOD) {
            return true;
        }

        return checkMemberAccessModifier(dynamic_cast<const ClassDeclarationNode*>(currentClass->getLocalNode()), memberName);
    }

    bool SemanticAnalyzer::checkMemberAccessModifier(const TypeDeclarationNode* receiverTypeDeclaration, std::string_view memberName, bool isSuperAccess) {
        const auto* typeDeclaration = receiverTypeDeclaration;
        if (typeDeclaration == nullptr) {
            return true;
        }

        std::optional<SymbolAccessModifier> accessModifier;
        const AstNode* memberDeclaration = nullptr;
        const auto getBaseClass = [this](const TypeDeclarationNode* typeDecl) -> const ClassDeclarationNode* {
            const auto* classDecl = dynamic_cast<const ClassDeclarationNode*>(typeDecl);
            if (classDecl == nullptr || !classDecl->getBaseClass().has_value()) {
                return nullptr;
            }

            const auto* baseType = context.getSemanticTypeByTypeNode(classDecl->getBaseClass().value().get());
            const auto* baseCustomizedType = dynamic_cast<const CustomizedType*>(baseType);
            if (baseCustomizedType == nullptr) {
                return nullptr;
            }
            if (baseCustomizedType->getOrigin() == CustomizedTypeOrigin::IMPORTED) {
                // TODO: Resolve imported base classes.
                return nullptr;
            }

            return dynamic_cast<const ClassDeclarationNode*>(baseCustomizedType->getLocalNode());
        };

        std::unordered_set<const TypeDeclarationNode*> visitedClasses;
        while (typeDeclaration != nullptr && visitedClasses.insert(typeDeclaration).second) {
            const auto* scope = context.getScopeByAstNode(typeDeclaration);
            if (scope != nullptr) {
                const Symbol* symbol = scope->lookupLocal(memberName);
                if (symbol != nullptr && (symbol->getKind() == SymbolKind::PROPERTY || symbol->getKind() == SymbolKind::METHOD || symbol->getKind() == SymbolKind::ENUM_MEMBER)) {
                    accessModifier = symbol->getAccessModifier();
                    memberDeclaration = symbol->getLocalNode();
                }
            } else if (const auto* classDecl = dynamic_cast<const ClassDeclarationNode*>(typeDeclaration)) {
                for (const auto& memberDecl : classDecl->getMemberDeclarations()) {
                    if (const auto* valueDecl = dynamic_cast<const ValueDeclarationNode*>(memberDecl.get())) {
                        if (valueDecl->getName().getIdentifierString() == memberName) {
                            accessModifier = static_cast<SymbolAccessModifier>(valueDecl->getAccessModifier());
                            memberDeclaration = valueDecl;
                            break;
                        }
                    } else if (const auto* funcDecl = dynamic_cast<const FunctionDeclarationNode*>(memberDecl.get())) {
                        if (funcDecl->getName().getIdentifierString() == memberName) {
                            accessModifier = static_cast<SymbolAccessModifier>(funcDecl->getAccessModifier());
                            memberDeclaration = funcDecl;
                            break;
                        }
                    }
                }
            } else if (const auto* interfaceDecl = dynamic_cast<const InterfaceDeclarationNode*>(typeDeclaration)) {
                for (const auto& methodDecl : interfaceDecl->getMethodDeclarations()) {
                    if (methodDecl->getName().getIdentifierString() == memberName) {
                        accessModifier = static_cast<SymbolAccessModifier>(methodDecl->getAccessModifier());
                        memberDeclaration = methodDecl.get();
                        break;
                    }
                }
            }

            if (accessModifier.has_value()) {
                break;
            }

            typeDeclaration = getBaseClass(typeDeclaration);
        }

        if (!accessModifier.has_value() || accessModifier.value() == SymbolAccessModifier::PUBLIC) {
            return true;
        }

        const Scope* currentClass = context.currentClass();
        if (currentClass == nullptr) {
            return false;
        }

        if (accessModifier.value() == SymbolAccessModifier::PRIVATE) {
            return typeDeclaration == currentClass->getLocalNode();
        }

        const auto isSameOrDerivedFrom = [&getBaseClass](const ClassDeclarationNode* classDecl, const TypeDeclarationNode* baseDecl) {
            std::unordered_set<const ClassDeclarationNode*> visitedDeclarations;
            while (classDecl != nullptr && visitedDeclarations.insert(classDecl).second) {
                if (classDecl == baseDecl) {
                    return true;
                }

                classDecl = getBaseClass(classDecl);
            }

            return false;
        };

        const auto* currentClassDeclaration = dynamic_cast<const ClassDeclarationNode*>(currentClass->getLocalNode());
        if (!isSameOrDerivedFrom(currentClassDeclaration, typeDeclaration)) {
            return false;
        }

        bool isStaticMember = false;
        if (const auto* valueDecl = dynamic_cast<const ValueDeclarationNode*>(memberDeclaration)) {
            isStaticMember = valueDecl->getKind() == ValueDeclarationType::Kind::STATIC_PROPERTY;
        } else if (const auto* funcDecl = dynamic_cast<const FunctionDeclarationNode*>(memberDeclaration)) {
            isStaticMember = funcDecl->getBinding() == FunctionDeclarationType::Binding::STATIC;
        }
        if (isStaticMember || isSuperAccess) {
            return true;
        }

        const auto* receiverClassDeclaration = dynamic_cast<const ClassDeclarationNode*>(receiverTypeDeclaration);
        return isSameOrDerivedFrom(receiverClassDeclaration, currentClassDeclaration);
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

    std::string SemanticAnalyzer::getFullTypeName(std::string_view typeName, const Config& config) const {
        return fmt::format("{}.{}", module.getFullName(), typeName);
    }

    bool SemanticAnalyzer::isActiveTypeDeclaration(const TypeDeclarationNode& typeDecl, std::string_view typeName) {
        const Symbol* symbol = context.currentScope().lookupLocal(typeName);
        return symbol != nullptr && symbol->getLocalNode() == &typeDecl;
    }

    const Scope* SemanticAnalyzer::getScopeBySymbol(const Symbol& symbol) const {
        if (symbol.getLocalNode() != nullptr) {
            return context.getScopeByAstNode(symbol.getLocalNode());
        }
        if (symbol.getImportedNode() != nullptr) {
            return context.getScopeByImportedNode(symbol.getImportedNode());
        }
        return nullptr;
    }

    void SemanticAnalyzer::registerLocalCustomizedType(const TypeDeclarationNode& typeDecl, std::string_view typeName, CustomizedTypeKind kind, const Config& config) {
        std::string fullTypeName = getFullTypeName(typeName, config);
        context.registerCustomizedType(fullTypeName, std::make_unique<CustomizedType>(kind, fullTypeName, &typeDecl));
    }

    void SemanticAnalyzer::checkModule(const ModuleNode& moduleNode, const Config& config) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::MODULE, nullptr, &moduleNode));

        for (const auto& importDecl : moduleNode.getImportDeclarations()) {
            checkImport(*importDecl, config);
        }

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(declNode)) {
                Symbol symbol(SymbolKind::VARIABLE, static_cast<SymbolAccessModifier>(varDecl->getAccessModifier()), varDecl->getName().getIdentifierString(), varDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*varDecl, fmt::format("Redeclaration of symbol '{}'", varDecl->getName().getIdentifierString()));
                }
            } else if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(declNode)) {
                Symbol symbol(SymbolKind::FUNCTION, static_cast<SymbolAccessModifier>(funcDecl->getAccessModifier()), funcDecl->getName().getIdentifierString(), funcDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*funcDecl, fmt::format("Redeclaration of symbol '{}'", funcDecl->getName().getIdentifierString()));
                }
            } else if (auto* classDecl = dynamic_cast<ClassDeclarationNode*>(declNode)) {
                Symbol symbol(SymbolKind::CLASS, SymbolAccessModifier::PUBLIC, classDecl->getName().getIdentifierString(), classDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*classDecl, fmt::format("Redeclaration of symbol '{}'", classDecl->getName().getIdentifierString()));
                }
            } else if (auto* interfaceDecl = dynamic_cast<InterfaceDeclarationNode*>(declNode)) {
                Symbol symbol(SymbolKind::INTERFACE, SymbolAccessModifier::PUBLIC, interfaceDecl->getName().getIdentifierString(), interfaceDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*interfaceDecl, fmt::format("Redeclaration of symbol '{}'", interfaceDecl->getName().getIdentifierString()));
                }
            } else if (auto* enumDecl = dynamic_cast<EnumDeclarationNode*>(declNode)) {
                Symbol symbol(SymbolKind::ENUM, SymbolAccessModifier::PUBLIC, enumDecl->getName().getIdentifierString(), enumDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*enumDecl, fmt::format("Redeclaration of symbol '{}'", enumDecl->getName().getIdentifierString()));
                }
            } else if (auto* typeAliasDecl = dynamic_cast<TypeAliasDeclarationNode*>(declNode)) {
                Symbol symbol(SymbolKind::TYPE_ALIAS, SymbolAccessModifier::PUBLIC, typeAliasDecl->getAliasName().getIdentifierString(), typeAliasDecl);
                if (!context.currentScope().declare(std::move(symbol))) {
                    context.reportError(*typeAliasDecl, fmt::format("Redeclaration of symbol '{}'", typeAliasDecl->getAliasName().getIdentifierString()));
                }
            }
        }

        for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
            DeclarationNode* declNode = topIdentifierDecl.get();

            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(declNode)) {
                if (varDecl->getKind() == ValueDeclarationType::Kind::LET) {
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

    void SemanticAnalyzer::checkImport(const ImportDeclarationNode& importDecl, const Config& config) {
        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> packages;
        const auto& importItem = importDecl.getNamePartsListWithAliases();
        try {
            PackageReader reader(packages);
            reader.readPackageFromSource(importItem, config);

            std::unordered_set<const ImportedModule*> visitedModules;
            std::unordered_set<std::string> loadedPaths;
            while (true) {
                std::unordered_set<std::string> dependencies;
                for (const auto& [name, package] : packages) {
                    collectImportDependencies(*package, dependencies, visitedModules);
                }
                if (dependencies.empty()) {
                    break;
                }
                for (const auto& dependency : dependencies) {
                    if (loadedPaths.insert(dependency).second) {
                        reader.readPackageFromAlias(dependency, config);
                    }
                }
            }
        } catch (const PackageReaderError& error) {
            context.reportError(error.locate() ? static_cast<const AstNode&>(*error.locate()) : importDecl, error.what());
            return;
        } catch (const ModuleInterfaceFileReaderError& error) {
            context.reportError(importDecl, error.what());
            return;
        } catch (const std::filesystem::filesystem_error& error) {
            context.reportError(importDecl, error.what());
            return;
        } catch (const nlohmann::json::exception& error) {
            context.reportError(importDecl, error.what());
            return;
        }

        struct ImportBinding {
            std::string name;
            std::vector<std::string> path;
            SymbolKind kind;
        };

        std::vector<ImportBinding> bindings;
        std::unordered_set<std::string> names;
        const auto errorCount = context.getErrors().size();

        const auto findChild = [](const ImportedItem* parent, std::string_view name) -> const ImportedItem* {
            if (const auto* package = dynamic_cast<const ImportedPackage*>(parent)) {
                if (auto subPackage = package->getSubPackageByName(name)) {
                    return subPackage;
                }
                return package->getModuleByName(name);
            }
            if (const auto* module = dynamic_cast<const ImportedModule*>(parent)) {
                return module->getIdentifierByName(name);
            }
            return nullptr;
        };

        const auto bind = [&](const ImportedItem* target, const std::vector<std::string>& path, const IdentifierNode* alias, const AstNode& location) {
            const std::string name(alias ? alias->getIdentifierString() : target->getName());
            if (context.currentScope().lookupLocal(name) != nullptr || !names.insert(name).second) {
                context.reportError(location, fmt::format("Redeclaration of symbol '{}'", name));
                return;
            }
            bindings.push_back({ name, path, getImportedSymbolKind(*target) });
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
                    if (target == nullptr) {
                        auto package = packages.find(std::string(name));
                        target = package == packages.end() ? nullptr : package->second.get();
                    } else {
                        target = findChild(target, name);
                    }
                    if (target == nullptr) {
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
        for (const auto& [name, stagedPackage] : packages) {
            const auto* package = context.getImportedPackageByName(name);
            if (package != nullptr && context.getScopeByImportedNode(package) != nullptr) {
                scopedPackages.insert(package);
            }
        }
        context.collectImportedPackages(std::move(packages));
        for (const auto& binding : bindings) {
            const auto* package = context.getImportedPackageByName(binding.path.front());
            const ImportedItem* target = package;
            for (std::size_t index = 1; index < binding.path.size(); ++index) {
                target = findChild(target, binding.path[index]);
            }
            context.currentScope().declare(Symbol(binding.kind, SymbolAccessModifier::PUBLIC, binding.name, target));
            if (getImportedScopeKind(*target).has_value()) {
                scopedPackages.insert(package);
            }
        }
        for (const auto* package : scopedPackages) {
            registerImportedScopes(*package, nullptr);
        }

        // TODO: implement importing subpackages, modules, or identifiers from .vnl in the same root package
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
        const bool requiresInitializer = kind == ValueDeclarationType::Kind::LET || kind == ValueDeclarationType::Kind::STATIC_PROPERTY;

        if (requiresInitializer && !hasInitializer) {
            context.reportError(varDecl, kind == ValueDeclarationType::Kind::STATIC_PROPERTY ? "Static properties must be initialized" : "Variables must be initialized");
        } else if (!requiresInitializer && hasInitializer) {
            context.reportError(
                varDecl,
                kind == ValueDeclarationType::Kind::INSTANCE_PROPERTY ? "Instance properties cannot have initializers" : "Value declarations of this kind cannot have initializers"
            );
        }

        if (hasInitializer) {
            checkExpression(*varDecl.getInitializer().value());
        }

        // TODO: Implement type checking and inference
    }

    void SemanticAnalyzer::checkFunctionDeclaration(const FunctionDeclarationNode& funcDecl, MetadataInfo metadataInfo) {
        context.pushScope(std::make_unique<Scope>(ScopeKind::FUNCTION, &context.currentScope(), &funcDecl));
        for (const auto& param : funcDecl.getParameters()) {
            Symbol paramSymbol(SymbolKind::PARAMETER, SymbolAccessModifier::PUBLIC, param->getName().getIdentifierString(), param.get());
            if (!context.currentScope().declare(std::move(paramSymbol))) {
                context.reportError(*param, fmt::format("Redeclaration of parameter '{}'", param->getName().getIdentifierString()));
            }
        }

        for (const auto& param : funcDecl.getParameters()) {
            checkValueDeclaration(*param);
        }

        if (funcDecl.getKind() == FunctionDeclarationType::Kind::REGULAR && funcDecl.getContext() != FunctionDeclarationType::Context::INTERFACE) {
            if (funcDecl.getBody().has_value()) {
                checkStatement(*funcDecl.getBody().value());
            } else {
                context.reportError(funcDecl, "Regular functions must have a body");
            }
        }

        // TODO: Implement return type checking and inference

        context.popScope();
    }

    void SemanticAnalyzer::checkClassDeclaration(const ClassDeclarationNode& classDecl, const Config& config, MetadataInfo metadataInfo) {
        const std::size_t errorCount = context.getErrors().size();
        context.pushScope(std::make_unique<Scope>(ScopeKind::CLASS, &context.currentScope(), &classDecl));

        for (const auto& member : classDecl.getMemberDeclarations()) {
            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(member.get())) {
                Symbol memberSymbol(SymbolKind::PROPERTY, static_cast<SymbolAccessModifier>(varDecl->getAccessModifier()), varDecl->getName().getIdentifierString(), varDecl);
                if (!context.currentScope().declare(std::move(memberSymbol))) {
                    context.reportError(*varDecl, fmt::format("Redeclaration of class member '{}'", varDecl->getName().getIdentifierString()));
                }
            } else if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(member.get())) {
                Symbol memberSymbol(SymbolKind::METHOD, static_cast<SymbolAccessModifier>(funcDecl->getAccessModifier()), funcDecl->getName().getIdentifierString(), funcDecl);
                if (!context.currentScope().declare(std::move(memberSymbol))) {
                    context.reportError(*funcDecl, fmt::format("Redeclaration of class member '{}'", funcDecl->getName().getIdentifierString()));
                }
            } else {
                context.reportError(*member, "Invalid class member declaration");
            }
        }

        for (const auto& genericParamName : classDecl.getGenericParameterNames()) {
            Symbol genericParamSymbol(SymbolKind::GENERIC_PARAMETER, SymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &classDecl);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(classDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& member : classDecl.getMemberDeclarations()) {
            if (auto* varDecl = dynamic_cast<ValueDeclarationNode*>(member.get())) {
                checkValueDeclaration(*varDecl);
            } else if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(member.get())) {
                checkFunctionDeclaration(*funcDecl);
            } else {
                context.reportError(*member, "Invalid class member declaration");
            }
        }

        context.popScope();

        if (context.getErrors().size() == errorCount && isActiveTypeDeclaration(classDecl, classDecl.getName().getIdentifierString())) {
            registerLocalCustomizedType(classDecl, classDecl.getName().getIdentifierString(), CustomizedTypeKind::CLASS, config);
        }
    }

    void SemanticAnalyzer::checkInterfaceDeclaration(const InterfaceDeclarationNode& interfaceDecl, const Config& config, MetadataInfo metadataInfo) {
        const std::size_t errorCount = context.getErrors().size();
        context.pushScope(std::make_unique<Scope>(ScopeKind::INTERFACE, &context.currentScope(), &interfaceDecl));

        for (const auto& member : interfaceDecl.getMethodDeclarations()) {
            Symbol memberSymbol(SymbolKind::METHOD, static_cast<SymbolAccessModifier>(member->getAccessModifier()), member->getName().getIdentifierString(), member.get());
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*member, fmt::format("Redeclaration of interface method '{}'", member->getName().getIdentifierString()));
            }
        }

        for (const auto& genericParamName : interfaceDecl.getGenericParameterNames()) {
            Symbol genericParamSymbol(SymbolKind::GENERIC_PARAMETER, SymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &interfaceDecl);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(interfaceDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& member : interfaceDecl.getMethodDeclarations()) {
            if (auto* funcDecl = dynamic_cast<FunctionDeclarationNode*>(member.get())) {
                checkFunctionDeclaration(*funcDecl);
            } else {
                context.reportError(*member, "Invalid interface member declaration");
            }
        }

        context.popScope();

        if (context.getErrors().size() == errorCount && isActiveTypeDeclaration(interfaceDecl, interfaceDecl.getName().getIdentifierString())) {
            registerLocalCustomizedType(interfaceDecl, interfaceDecl.getName().getIdentifierString(), CustomizedTypeKind::INTERFACE, config);
        }
    }

    void SemanticAnalyzer::checkEnumDeclaration(const EnumDeclarationNode& enumDecl, const Config& config, MetadataInfo metadataInfo) {
        const std::size_t errorCount = context.getErrors().size();
        context.pushScope(std::make_unique<Scope>(ScopeKind::ENUM, &context.currentScope(), &enumDecl));

        for (const auto& member : enumDecl.getMemberDeclarations()) {
            Symbol memberSymbol(SymbolKind::ENUM_MEMBER, SymbolAccessModifier::PUBLIC, member->getName().getIdentifierString(), member.get());
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*member, fmt::format("Redeclaration of enum member '{}'", member->getName().getIdentifierString()));
            }
        }

        for (const auto& genericParamName : enumDecl.getGenericParameterNames()) {
            Symbol genericParamSymbol(SymbolKind::GENERIC_PARAMETER, SymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &enumDecl);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(enumDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        for (const auto& member : enumDecl.getMemberDeclarations()) {
            context.pushScope(std::make_unique<Scope>(ScopeKind::ENUM_MEMBER, &context.currentScope(), member.get()));

            for (auto& associatedValue : member->getAssociatedValues()) {
                Symbol associatedValueSymbol(
                    SymbolKind::PROPERTY,

                    SymbolAccessModifier::PUBLIC,
                    associatedValue->getName().getIdentifierString(),
                    associatedValue.get()
                );
                if (!context.currentScope().declare(std::move(associatedValueSymbol))) {
                    context.reportError(*associatedValue, fmt::format("Redeclaration of enum member associated value '{}'", associatedValue->getName().getIdentifierString()));
                }
            }

            for (auto& associatedValue : member->getAssociatedValues()) {
                checkValueDeclaration(*associatedValue);
            }

            context.popScope();
        }

        context.popScope();

        if (context.getErrors().size() == errorCount && isActiveTypeDeclaration(enumDecl, enumDecl.getName().getIdentifierString())) {
            registerLocalCustomizedType(enumDecl, enumDecl.getName().getIdentifierString(), CustomizedTypeKind::ENUM, config);
        }
    }

    void SemanticAnalyzer::checkTypeAliasDeclaration(const TypeAliasDeclarationNode& typeAliasDecl, const Config& config, MetadataInfo metadataInfo) {
        const std::size_t errorCount = context.getErrors().size();
        context.pushScope(std::make_unique<Scope>(ScopeKind::TYPE_ALIAS, &context.currentScope(), &typeAliasDecl));

        for (const auto& genericParamName : typeAliasDecl.getGenericParameterNames()) {
            Symbol genericParamSymbol(SymbolKind::GENERIC_PARAMETER, SymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &typeAliasDecl);
            if (!context.currentScope().declare(std::move(genericParamSymbol))) {
                context.reportError(typeAliasDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
            }
        }

        checkType(typeAliasDecl.getOriginalType());

        context.popScope();

        if (context.getErrors().size() == errorCount && isActiveTypeDeclaration(typeAliasDecl, typeAliasDecl.getAliasName().getIdentifierString())) {
            registerLocalCustomizedType(typeAliasDecl, typeAliasDecl.getAliasName().getIdentifierString(), CustomizedTypeKind::TYPE_ALIAS, config);
        }
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
                const Symbol* labelSymbol = context.currentScope().lookup(label.value()->getIdentifierString());
                if (labelSymbol == nullptr) {
                    context.reportError(*stmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
                } else if (labelSymbol->getKind() != SymbolKind::LOOP_LABEL) {
                    context.reportError(*stmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
                }
            }
        } else if (auto* stmt = dynamic_cast<const ContinueStatementNode*>(&statement)) {
            if (!context.currentLoop()) {
                context.reportError(*stmt, "Continue statement not within a loop");
            }

            const auto& label = stmt->getLabel();
            if (label.has_value()) {
                const Symbol* labelSymbol = context.currentScope().lookup(label.value()->getIdentifierString());
                if (labelSymbol == nullptr) {
                    context.reportError(*stmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
                } else if (labelSymbol->getKind() != SymbolKind::LOOP_LABEL) {
                    context.reportError(*stmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
                }
            }
        } else if (auto* stmt = dynamic_cast<const ExpressionStatementNode*>(&statement)) {
            checkExpression(stmt->getExpression());
        } else if (auto* stmt = dynamic_cast<const ForStatementNode*>(&statement)) {
            context.pushScope(std::make_unique<Scope>(ScopeKind::LOOP, &context.currentScope(), stmt));

            const auto& label = stmt->getLabel();
            if (label.has_value()) {
                Symbol labelSymbol(SymbolKind::LOOP_LABEL, SymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), stmt);

                if (!context.currentScope().declare(std::move(labelSymbol))) {
                    context.reportError(*stmt, fmt::format("Redeclaration of identifier '{}'", label.value()->getIdentifierString()));
                }
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

            if (stmt->getSwitchType() == SwitchStatementType::LITERAL_MATCH) {
                for (const auto& item : stmt->getLiteralMatchItems()) {
                    checkStatement(*item.body);
                }
            } else if (stmt->getSwitchType() == SwitchStatementType::TYPE_MATCH) {
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
            checkValueDeclaration(stmt->getVariableDeclaration());
        } else if (auto* stmt = dynamic_cast<const WhileStatementNode*>(&statement)) {
            context.pushScope(std::make_unique<Scope>(ScopeKind::LOOP, &context.currentScope(), stmt));

            const auto& label = stmt->getLabel();
            if (label.has_value()) {
                Symbol labelSymbol(SymbolKind::LOOP_LABEL, SymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), stmt);

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
        // TODO: Implement expression checking process
    }

    void SemanticAnalyzer::checkType(const TypeNode& type) {
        const auto& nameParts = type.getNameParts();
        const auto isPrimitiveType = [](std::string_view name) {
            static const std::unordered_set<std::string_view> primitiveTypes = {
                "byte", "short", "int", "long", "float", "double", "bool", "string",
            };
            return primitiveTypes.contains(name);
        };
        const auto isTypeDefinition = [](SymbolKind kind) {
            switch (kind) {
                case SymbolKind::CLASS:
                case SymbolKind::INTERFACE:
                case SymbolKind::ENUM:
                case SymbolKind::ENUM_MEMBER:
                case SymbolKind::TYPE_ALIAS:
                case SymbolKind::GENERIC_PARAMETER:
                    return true;
                default:
                    return false;
            }
        };

        if (nameParts.size() != 1 || !isPrimitiveType(nameParts.front()->getIdentifierString())) {
            const Scope* scope = &context.currentScope();
            for (std::size_t index = 0; index < nameParts.size(); ++index) {
                const auto& namePart = nameParts[index];
                const Symbol* symbol = scope->lookup(namePart->getIdentifierString());
                if (symbol == nullptr) {
                    context.reportError(*namePart, fmt::format("Use of undeclared type '{}'", namePart->getIdentifierString()));
                    break;
                }

                if (index == nameParts.size() - 1) {
                    if (!isTypeDefinition(symbol->getKind())) {
                        context.reportError(*namePart, fmt::format("Identifier '{}' is not a type definition", namePart->getIdentifierString()));
                    }
                    break;
                }

                scope = getScopeBySymbol(*symbol);
                if (scope == nullptr) {
                    context.reportError(*namePart, fmt::format("Identifier '{}' does not have a scope", namePart->getIdentifierString()));
                    break;
                }
            }
        }

        for (const auto& genericArgument : type.getGenericArguments()) {
            checkType(*genericArgument);
        }
    }

    TypeInferenceResult SemanticAnalyzer::inferExpressionType(const ExpressionNode& expression) {
        // TODO: Implement expression type inference

        return TypeInferenceResult::failed();
    }

    TypeInferenceResult SemanticAnalyzer::inferFunctionReturnType(const FunctionDeclarationNode& funcDecl) {
        // TODO: Implement function return type inference

        return TypeInferenceResult::failed();
    }

    SemanticAnalysisResult SemanticAnalyzer::analyze(const Config& config) {
        checkModule(module, config);

        auto diagnostics = context.takeDiagnostics();
        auto customizedTypes = context.takeCustomizedTypeRegistry();
        auto scopes = context.takeLocalScopeMap();
        auto importedScopes = context.takeImportedScopeMap();
        auto semanticTypes = context.takeSemanticTypeMap();
        auto inferredValueTypes = context.takeInferredValueTypeMap();
        auto inferredFunctionReturnTypes = context.takeInferredFunctionReturnTypeMap();
        auto inferredExpressionTypes = context.takeInferredExpressionTypeMap();
        auto importedPackages = context.takeImportedPackages();
        return SemanticAnalysisResult(
            std::move(std::get<0>(diagnostics)),
            std::move(std::get<1>(diagnostics)),
            std::move(std::get<2>(diagnostics)),
            std::move(customizedTypes),
            std::move(scopes),
            std::move(importedScopes),
            std::move(semanticTypes),
            std::move(inferredValueTypes),
            std::move(inferredFunctionReturnTypes),
            std::move(inferredExpressionTypes),
            std::move(importedPackages)
        );
    }
} // namespace vnlc
