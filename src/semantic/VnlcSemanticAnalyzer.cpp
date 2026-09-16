#include "VnlcSemanticAnalyzer.hpp"
#include "ast/expression/VnlcIdentifierExpressionNode.hpp"
#include "ast/expression/VnlcMemberAccessExpressionNode.hpp"
#include "ast/expression/VnlcSuperExpressionNode.hpp"
#include "ast/statement/VnlcBlockStatementNode.hpp"
#include "ast/statement/VnlcBreakStatementNode.hpp"
#include "ast/statement/VnlcContinueStatementNode.hpp"
#include "ast/statement/VnlcExpressionStatementNode.hpp"
#include "ast/statement/VnlcForStatementNode.hpp"
#include "ast/statement/VnlcIfStatementNode.hpp"
#include "ast/statement/VnlcReturnStatementNode.hpp"
#include "ast/statement/VnlcSwitchStatementNode.hpp"
#include "ast/statement/VnlcVariableDeclarationStatementNode.hpp"
#include "ast/statement/VnlcWhileStatementNode.hpp"
#include "error/VnlcModuleInterfaceReaderError.hpp"
#include "error/VnlcPackageReaderError.hpp"
#include "symbol/VnlcSymbolAccessModifier.hpp"
#include "symbol/VnlcSymbolKind.hpp"
#include "type/VnlcCustomizedType.hpp"
#include "type/VnlcCustomizedTypeOrigin.hpp"
#include "type/VnlcSemanticType.hpp"
#include "type/VnlcTypeExpressionType.hpp"
#include "type/typeinf/VnlcTypeInferenceResult.hpp"
#include "vni/import/VnlcImportedClass.hpp"
#include "vni/import/VnlcImportedEnum.hpp"
#include "vni/import/VnlcImportedEnumMember.hpp"
#include "vni/import/VnlcImportedFunc.hpp"
#include "vni/import/VnlcImportedInterface.hpp"
#include "vni/import/VnlcImportedLet.hpp"
#include "vni/import/VnlcImportedMethod.hpp"
#include "vni/import/VnlcImportedParameter.hpp"
#include "vni/import/VnlcImportedProperty.hpp"
#include "vni/import/VnlcImportedTypeAlias.hpp"
#include "vni/import/VnlcPackageReader.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>

VnlcSemanticAnalyzer::VnlcSemanticAnalyzer(const VnlcModuleNode& module) : module(module) {}

void VnlcSemanticAnalyzer::checkIdentifierExpressionUse(const VnlcIdentifierExpressionNode& exprNode, VnlcMetadataInfo metadataInfo) {
    auto symbol = context.currentScope().lookup(exprNode.getName().getIdentifierString());
    if (!symbol.has_value()) {
        context.reportError(exprNode, fmt::format("Use of undeclared identifier '{}'", exprNode.getName().getIdentifierString()));
    } else if (!(dynamic_cast<const VnlcValueDeclarationNode*>(symbol.value()->getLocalNode()) || dynamic_cast<const VnlcFunctionDeclarationNode*>(symbol.value()->getLocalNode()))) {
        context.reportError(exprNode, fmt::format("Identifier '{}' is not a variable or function", exprNode.getName().getIdentifierString()));
    }
}

bool VnlcSemanticAnalyzer::checkAccessModifier(const VnlcMemberAccessExpressionNode& memberAccessNode) {
    const auto& prefix = memberAccessNode.getObject();
    const auto& member = memberAccessNode.getMember();

    const VnlcSemanticType* prefixType = context.getInferredExpressionType(&prefix).value_or(nullptr);
    if (const auto* typeExpression = dynamic_cast<const VnlcTypeExpressionType*>(prefixType)) {
        prefixType = typeExpression->getExpressedType();
    }

    const auto* customizedType = dynamic_cast<const VnlcCustomizedType*>(prefixType);
    if (customizedType == nullptr) {
        return true;
    }
    if (customizedType->getOrigin() == VnlcCustomizedTypeOrigin::IMPORTED) {
        // TODO: Check access to imported members.
        return true;
    }

    return checkMemberAccessModifier(customizedType->getLocalNode(), member.getName().getIdentifierString(), dynamic_cast<const VnlcSuperExpressionNode*>(&prefix) != nullptr);
}

bool VnlcSemanticAnalyzer::checkAccessModifier(const VnlcIdentifierExpressionNode& identifierNode) {
    const auto* currentClass = context.currentClass();
    if (currentClass == nullptr) {
        return true;
    }

    const auto memberName = identifierNode.getName().getIdentifierString();
    for (const auto* scope = &context.currentScope(); scope != nullptr && scope != currentClass; scope = scope->findParent()) {
        if (scope->lookupLocal(memberName).has_value()) {
            return true;
        }
    }

    const auto symbol = currentClass->lookupLocal(memberName);
    if (symbol.has_value() && symbol.value()->getKind() != VnlcSymbolKind::PROPERTY && symbol.value()->getKind() != VnlcSymbolKind::METHOD) {
        return true;
    }

    return checkMemberAccessModifier(dynamic_cast<const VnlcClassDeclarationNode*>(currentClass->getLocalNode()), memberName);
}

bool VnlcSemanticAnalyzer::checkMemberAccessModifier(const VnlcTypeDeclarationNode* receiverTypeDeclaration, std::string_view memberName, bool isSuperAccess) {
    const auto* typeDeclaration = receiverTypeDeclaration;
    if (typeDeclaration == nullptr) {
        return true;
    }

    std::optional<VnlcSymbolAccessModifier> accessModifier;
    const VnlcAstNode* memberDeclaration = nullptr;
    const auto getBaseClass = [this](const VnlcTypeDeclarationNode* typeDecl) -> const VnlcClassDeclarationNode* {
        const auto* classDecl = dynamic_cast<const VnlcClassDeclarationNode*>(typeDecl);
        if (classDecl == nullptr || !classDecl->getBaseClass().has_value()) {
            return nullptr;
        }

        const auto* baseType = context.getSemanticTypeByTypeNode(classDecl->getBaseClass().value().get()).value_or(nullptr);
        const auto* baseCustomizedType = dynamic_cast<const VnlcCustomizedType*>(baseType);
        if (baseCustomizedType == nullptr) {
            return nullptr;
        }
        if (baseCustomizedType->getOrigin() == VnlcCustomizedTypeOrigin::IMPORTED) {
            // TODO: Resolve imported base classes.
            return nullptr;
        }

        return dynamic_cast<const VnlcClassDeclarationNode*>(baseCustomizedType->getLocalNode());
    };

    std::unordered_set<const VnlcTypeDeclarationNode*> visitedClasses;
    while (typeDeclaration != nullptr && visitedClasses.insert(typeDeclaration).second) {
        const auto* scope = context.getScopeByAstNode(typeDeclaration);
        if (scope != nullptr) {
            const auto symbol = scope->lookupLocal(memberName);
            if (symbol.has_value() &&
                (symbol.value()->getKind() == VnlcSymbolKind::PROPERTY || symbol.value()->getKind() == VnlcSymbolKind::METHOD || symbol.value()->getKind() == VnlcSymbolKind::ENUM_MEMBER)) {
                accessModifier = symbol.value()->getAccessModifier();
                memberDeclaration = symbol.value()->getLocalNode();
            }
        } else if (const auto* classDecl = dynamic_cast<const VnlcClassDeclarationNode*>(typeDeclaration)) {
            for (const auto& memberDecl : classDecl->getMemberDeclarations()) {
                if (const auto* valueDecl = dynamic_cast<const VnlcValueDeclarationNode*>(memberDecl.get())) {
                    if (valueDecl->getName().getIdentifierString() == memberName) {
                        accessModifier = static_cast<VnlcSymbolAccessModifier>(valueDecl->getAccessModifier());
                        memberDeclaration = valueDecl;
                        break;
                    }
                } else if (const auto* funcDecl = dynamic_cast<const VnlcFunctionDeclarationNode*>(memberDecl.get())) {
                    if (funcDecl->getName().getIdentifierString() == memberName) {
                        accessModifier = static_cast<VnlcSymbolAccessModifier>(funcDecl->getAccessModifier());
                        memberDeclaration = funcDecl;
                        break;
                    }
                }
            }
        } else if (const auto* interfaceDecl = dynamic_cast<const VnlcInterfaceDeclarationNode*>(typeDeclaration)) {
            for (const auto& methodDecl : interfaceDecl->getMethodDeclarations()) {
                if (methodDecl->getName().getIdentifierString() == memberName) {
                    accessModifier = static_cast<VnlcSymbolAccessModifier>(methodDecl->getAccessModifier());
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

    if (!accessModifier.has_value() || accessModifier.value() == VnlcSymbolAccessModifier::PUBLIC) {
        return true;
    }

    const VnlcScope* currentClass = context.currentClass();
    if (currentClass == nullptr) {
        return false;
    }

    if (accessModifier.value() == VnlcSymbolAccessModifier::PRIVATE) {
        return typeDeclaration == currentClass->getLocalNode();
    }

    const auto isSameOrDerivedFrom = [&getBaseClass](const VnlcClassDeclarationNode* classDecl, const VnlcTypeDeclarationNode* baseDecl) {
        std::unordered_set<const VnlcClassDeclarationNode*> visitedDeclarations;
        while (classDecl != nullptr && visitedDeclarations.insert(classDecl).second) {
            if (classDecl == baseDecl) {
                return true;
            }

            classDecl = getBaseClass(classDecl);
        }

        return false;
    };

    const auto* currentClassDeclaration = dynamic_cast<const VnlcClassDeclarationNode*>(currentClass->getLocalNode());
    if (!isSameOrDerivedFrom(currentClassDeclaration, typeDeclaration)) {
        return false;
    }

    bool isStaticMember = false;
    if (const auto* valueDecl = dynamic_cast<const VnlcValueDeclarationNode*>(memberDeclaration)) {
        isStaticMember = valueDecl->getKind() == VnlcValueDeclarationType::Kind::STATIC_PROPERTY;
    } else if (const auto* funcDecl = dynamic_cast<const VnlcFunctionDeclarationNode*>(memberDeclaration)) {
        isStaticMember = funcDecl->getBinding() == VnlcFunctionDeclarationType::Binding::STATIC;
    }
    if (isStaticMember || isSuperAccess) {
        return true;
    }

    const auto* receiverClassDeclaration = dynamic_cast<const VnlcClassDeclarationNode*>(receiverTypeDeclaration);
    return isSameOrDerivedFrom(receiverClassDeclaration, currentClassDeclaration);
}

VnlcMetadataInfo VnlcSemanticAnalyzer::checkMetadata(const std::vector<VnlcDeclarationItem::MetadataTerm>& metadataTerms, const VnlcDeclarationNode& declNode) {
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

    return VnlcMetadataInfo{
        noWarnings,
        deprecated,
    };
}

std::string VnlcSemanticAnalyzer::getFullTypeName(std::string_view typeName, const VnlcConfig& config) const {
    return fmt::format("{}.{}", module.getFullName(), typeName);
}

bool VnlcSemanticAnalyzer::isActiveTypeDeclaration(const VnlcTypeDeclarationNode& typeDecl, std::string_view typeName) {
    const auto symbol = context.currentScope().lookupLocal(typeName);
    return symbol.has_value() && symbol.value()->getLocalNode() == &typeDecl;
}

void VnlcSemanticAnalyzer::registerLocalCustomizedType(const VnlcTypeDeclarationNode& typeDecl, std::string_view typeName, VnlcCustomizedTypeKind kind, const VnlcConfig& config) {
    std::string fullTypeName = getFullTypeName(typeName, config);
    context.registerCustomizedType(fullTypeName, std::make_unique<VnlcCustomizedType>(kind, fullTypeName, &typeDecl));
}

void VnlcSemanticAnalyzer::checkModule(const VnlcModuleNode& moduleNode, const VnlcConfig& config) {
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::MODULE, nullptr, &moduleNode));

    for (const auto& importDecl : moduleNode.getImportDeclarations()) {
        checkImport(*importDecl, config);
    }

    for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
        VnlcDeclarationNode* declNode = topIdentifierDecl.get();

        if (auto* varDecl = dynamic_cast<VnlcValueDeclarationNode*>(declNode)) {
            VnlcSymbol symbol(VnlcSymbolKind::VARIABLE, static_cast<VnlcSymbolAccessModifier>(varDecl->getAccessModifier()), varDecl->getName().getIdentifierString(), varDecl);
            if (!context.currentScope().declare(std::move(symbol))) {
                context.reportError(*varDecl, fmt::format("Redeclaration of symbol '{}'", varDecl->getName().getIdentifierString()));
            }
        } else if (auto* funcDecl = dynamic_cast<VnlcFunctionDeclarationNode*>(declNode)) {
            VnlcSymbol symbol(VnlcSymbolKind::FUNCTION, static_cast<VnlcSymbolAccessModifier>(funcDecl->getAccessModifier()), funcDecl->getName().getIdentifierString(), funcDecl);
            if (!context.currentScope().declare(std::move(symbol))) {
                context.reportError(*funcDecl, fmt::format("Redeclaration of symbol '{}'", funcDecl->getName().getIdentifierString()));
            }
        } else if (auto* classDecl = dynamic_cast<VnlcClassDeclarationNode*>(declNode)) {
            VnlcSymbol symbol(VnlcSymbolKind::CLASS, VnlcSymbolAccessModifier::PUBLIC, classDecl->getName().getIdentifierString(), classDecl);
            if (!context.currentScope().declare(std::move(symbol))) {
                context.reportError(*classDecl, fmt::format("Redeclaration of symbol '{}'", classDecl->getName().getIdentifierString()));
            }
        } else if (auto* interfaceDecl = dynamic_cast<VnlcInterfaceDeclarationNode*>(declNode)) {
            VnlcSymbol symbol(VnlcSymbolKind::INTERFACE, VnlcSymbolAccessModifier::PUBLIC, interfaceDecl->getName().getIdentifierString(), interfaceDecl);
            if (!context.currentScope().declare(std::move(symbol))) {
                context.reportError(*interfaceDecl, fmt::format("Redeclaration of symbol '{}'", interfaceDecl->getName().getIdentifierString()));
            }
        } else if (auto* enumDecl = dynamic_cast<VnlcEnumDeclarationNode*>(declNode)) {
            VnlcSymbol symbol(VnlcSymbolKind::ENUM, VnlcSymbolAccessModifier::PUBLIC, enumDecl->getName().getIdentifierString(), enumDecl);
            if (!context.currentScope().declare(std::move(symbol))) {
                context.reportError(*enumDecl, fmt::format("Redeclaration of symbol '{}'", enumDecl->getName().getIdentifierString()));
            }
        } else if (auto* typeAliasDecl = dynamic_cast<VnlcTypeAliasDeclarationNode*>(declNode)) {
            VnlcSymbol symbol(VnlcSymbolKind::TYPE_ALIAS, VnlcSymbolAccessModifier::PUBLIC, typeAliasDecl->getAliasName().getIdentifierString(), typeAliasDecl);
            if (!context.currentScope().declare(std::move(symbol))) {
                context.reportError(*typeAliasDecl, fmt::format("Redeclaration of symbol '{}'", typeAliasDecl->getAliasName().getIdentifierString()));
            }
        }
    }

    for (const auto& topIdentifierDecl : moduleNode.getTopIdentifierDeclarations()) {
        VnlcDeclarationNode* declNode = topIdentifierDecl.get();

        if (auto* varDecl = dynamic_cast<VnlcValueDeclarationNode*>(declNode)) {
            if (varDecl->getKind() == VnlcValueDeclarationType::Kind::LET) {
                checkValueDeclaration(*varDecl);
            } else {
                context.reportError(*varDecl, fmt::format("Top-level value declaration must be a variable"));
            }
        } else if (auto* funcDecl = dynamic_cast<VnlcFunctionDeclarationNode*>(declNode)) {
            checkFunctionDeclaration(*funcDecl);
        } else if (auto* classDecl = dynamic_cast<VnlcClassDeclarationNode*>(declNode)) {
            checkClassDeclaration(*classDecl, config);
        } else if (auto* interfaceDecl = dynamic_cast<VnlcInterfaceDeclarationNode*>(declNode)) {
            checkInterfaceDeclaration(*interfaceDecl, config);
        } else if (auto* enumDecl = dynamic_cast<VnlcEnumDeclarationNode*>(declNode)) {
            checkEnumDeclaration(*enumDecl, config);
        } else if (auto* typeAliasDecl = dynamic_cast<VnlcTypeAliasDeclarationNode*>(declNode)) {
            checkTypeAliasDeclaration(*typeAliasDecl, config);
        }
    }

    for (const auto& exportDecl : moduleNode.getExportDeclarations()) {
        checkExport(*exportDecl);
    }

    context.popScope();
}

void VnlcSemanticAnalyzer::checkImport(const VnlcImportDeclarationNode& importDecl, const VnlcConfig& config) {
    std::unordered_map<std::string, std::unique_ptr<VnlcImportedPackage>> packages;
    const auto& importItem = importDecl.getNamePartsListWithAliases();
    try {
        VnlcPackageReader reader(packages);
        reader.readPackage(importItem, config);
    } catch (const VnlcPackageReaderError& error) {
        context.reportError(error.locate() ? static_cast<const VnlcAstNode&>(*error.locate()) : importDecl, error.what());
        return;
    } catch (const VnlcModuleInterfaceFileReaderError& error) {
        context.reportError(importDecl, error.what());
        return;
    } catch (const std::filesystem::filesystem_error& error) {
        context.reportError(importDecl, error.what());
        return;
    } catch (const nlohmann::json::exception& error) {
        context.reportError(importDecl, error.what());
        return;
    }

    struct VnlcImportBinding {
        std::string name;
        std::vector<std::string> path;
        VnlcSymbolKind kind;
    };

    std::vector<VnlcImportBinding> bindings;
    std::unordered_set<std::string> names;
    const auto errorCount = context.getErrors().size();

    const auto findChild = [](const VnlcImportedItem* parent, std::string_view name) -> const VnlcImportedItem* {
        if (const auto* package = dynamic_cast<const VnlcImportedPackage*>(parent)) {
            if (auto subPackage = package->getSubPackageByName(name)) {
                return subPackage.value();
            }
            return package->getModuleByName(name).value_or(nullptr);
        }
        if (const auto* module = dynamic_cast<const VnlcImportedModule*>(parent)) {
            return module->getIdentifierByName(name).value_or(nullptr);
        }
        return nullptr;
    };

    const auto getKind = [](const VnlcImportedItem* item) {
        if (dynamic_cast<const VnlcImportedPackage*>(item)) return VnlcSymbolKind::PACKAGE;
        if (dynamic_cast<const VnlcImportedModule*>(item)) return VnlcSymbolKind::MODULE;
        if (dynamic_cast<const VnlcImportedLet*>(item)) return VnlcSymbolKind::VARIABLE;
        if (dynamic_cast<const VnlcImportedFunc*>(item)) return VnlcSymbolKind::FUNCTION;
        if (dynamic_cast<const VnlcImportedClass*>(item)) return VnlcSymbolKind::CLASS;
        if (dynamic_cast<const VnlcImportedInterface*>(item)) return VnlcSymbolKind::INTERFACE;
        if (dynamic_cast<const VnlcImportedEnum*>(item)) return VnlcSymbolKind::ENUM;
        if (dynamic_cast<const VnlcImportedTypeAlias*>(item)) return VnlcSymbolKind::TYPE_ALIAS;
        if (dynamic_cast<const VnlcImportedEnumMember*>(item)) return VnlcSymbolKind::ENUM_MEMBER;
        if (dynamic_cast<const VnlcImportedProperty*>(item)) return VnlcSymbolKind::PROPERTY;
        if (dynamic_cast<const VnlcImportedMethod*>(item)) return VnlcSymbolKind::METHOD;
        if (dynamic_cast<const VnlcImportedParameter*>(item)) return VnlcSymbolKind::PARAMETER;
        return VnlcSymbolKind::IMPORT_ALIAS;
    };

    const auto bind = [&](const VnlcImportedItem* target, const std::vector<std::string>& path, const VnlcIdentifierNode* alias, const VnlcAstNode& location) {
        const std::string name(alias ? alias->getIdentifierString() : target->getName());
        if (context.currentScope().lookupLocal(name).has_value() || !names.insert(name).second) {
            context.reportError(location, fmt::format("Redeclaration of symbol '{}'", name));
            return;
        }
        bindings.push_back({ name, path, alias ? VnlcSymbolKind::IMPORT_ALIAS : getKind(target) });
    };

    std::function<void(const VnlcImportDeclarationItem&, const VnlcImportedItem*, std::vector<std::string>)> checkItem;
    checkItem = [&](const VnlcImportDeclarationItem& item, const VnlcImportedItem* parent, std::vector<std::string> path) {
        const VnlcAstNode& location = item.namePrefix.empty() ? static_cast<const VnlcAstNode&>(importDecl) : *item.namePrefix.back();
        const VnlcImportedItem* target = parent;
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
        if (item.self && !dynamic_cast<const VnlcImportedPackage*>(target) && !dynamic_cast<const VnlcImportedModule*>(target)) {
            context.reportError(location, "Self imports can only refer to packages or modules");
            return;
        }
        if (item.wildcard) {
            const auto* importedModule = dynamic_cast<const VnlcImportedModule*>(target);
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
                bind(importedModule->getIdentifierByName(name).value(), path, nullptr, location);
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

    context.collectImportedPackages(std::move(packages));
    for (const auto& binding : bindings) {
        const VnlcImportedItem* target = context.getImportedPackageByName(binding.path.front()).value();
        for (std::size_t index = 1; index < binding.path.size(); ++index) {
            target = findChild(target, binding.path[index]);
        }
        context.mapImportedBinding(binding.name, target);
        context.currentScope().declare(VnlcSymbol(binding.kind, VnlcSymbolAccessModifier::PUBLIC, binding.name, target));
    }
}

void VnlcSemanticAnalyzer::checkExport(const VnlcExportDeclarationNode& exportDecl) {
    for (auto& item : exportDecl.getNameList()) {
        if (!context.currentScope().lookup(item.name->getIdentifierString()).has_value()) {
            context.reportError(exportDecl, fmt::format("Undefined symbol {}", item.name->getIdentifierString()));
        }
    }
}

void VnlcSemanticAnalyzer::checkValueDeclaration(const VnlcValueDeclarationNode& varDecl, VnlcMetadataInfo metadataInfo) {
    auto kind = varDecl.getKind();
    const bool hasInitializer = varDecl.getInitializer().has_value();
    const bool requiresInitializer = kind == VnlcValueDeclarationType::Kind::LET || kind == VnlcValueDeclarationType::Kind::STATIC_PROPERTY;

    if (requiresInitializer && !hasInitializer) {
        context.reportError(varDecl, kind == VnlcValueDeclarationType::Kind::STATIC_PROPERTY ? "Static properties must be initialized" : "Variables must be initialized");
    } else if (!requiresInitializer && hasInitializer) {
        context.reportError(
            varDecl,
            kind == VnlcValueDeclarationType::Kind::INSTANCE_PROPERTY ? "Instance properties cannot have initializers" : "Value declarations of this kind cannot have initializers"
        );
    }

    if (hasInitializer) {
        checkExpression(*varDecl.getInitializer().value());
    }

    // TODO: Implement type checking and inference
}

void VnlcSemanticAnalyzer::checkFunctionDeclaration(const VnlcFunctionDeclarationNode& funcDecl, VnlcMetadataInfo metadataInfo) {
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::FUNCTION, &context.currentScope(), &funcDecl));
    for (const auto& param : funcDecl.getParameters()) {
        VnlcSymbol paramSymbol(VnlcSymbolKind::PARAMETER, VnlcSymbolAccessModifier::PUBLIC, param->getName().getIdentifierString(), param.get());
        if (!context.currentScope().declare(std::move(paramSymbol))) {
            context.reportError(*param, fmt::format("Redeclaration of parameter '{}'", param->getName().getIdentifierString()));
        }
    }

    for (const auto& param : funcDecl.getParameters()) {
        checkValueDeclaration(*param);
    }

    if (funcDecl.getKind() == VnlcFunctionDeclarationType::Kind::REGULAR && funcDecl.getContext() != VnlcFunctionDeclarationType::Context::INTERFACE) {
        if (funcDecl.getBody().has_value()) {
            checkStatement(*funcDecl.getBody().value());
        } else {
            context.reportError(funcDecl, "Regular functions must have a body");
        }
    }

    // TODO: Implement return type checking and inference

    context.popScope();
}

void VnlcSemanticAnalyzer::checkClassDeclaration(const VnlcClassDeclarationNode& classDecl, const VnlcConfig& config, VnlcMetadataInfo metadataInfo) {
    const std::size_t errorCount = context.getErrors().size();
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::CLASS, &context.currentScope(), &classDecl));

    for (const auto& member : classDecl.getMemberDeclarations()) {
        if (auto* varDecl = dynamic_cast<VnlcValueDeclarationNode*>(member.get())) {
            VnlcSymbol memberSymbol(
                VnlcSymbolKind::PROPERTY,

                static_cast<VnlcSymbolAccessModifier>(varDecl->getAccessModifier()),
                varDecl->getName().getIdentifierString(),
                varDecl
            );
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*varDecl, fmt::format("Redeclaration of class member '{}'", varDecl->getName().getIdentifierString()));
            }
        } else if (auto* funcDecl = dynamic_cast<VnlcFunctionDeclarationNode*>(member.get())) {
            VnlcSymbol memberSymbol(
                VnlcSymbolKind::METHOD,

                static_cast<VnlcSymbolAccessModifier>(funcDecl->getAccessModifier()),
                funcDecl->getName().getIdentifierString(),
                funcDecl
            );
            if (!context.currentScope().declare(std::move(memberSymbol))) {
                context.reportError(*funcDecl, fmt::format("Redeclaration of class member '{}'", funcDecl->getName().getIdentifierString()));
            }
        } else {
            context.reportError(*member, "Invalid class member declaration");
        }
    }

    for (const auto& genericParamName : classDecl.getGenericParameterNames()) {
        VnlcSymbol genericParamSymbol(VnlcSymbolKind::GENERIC_PARAMETER, VnlcSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &classDecl);
        if (!context.currentScope().declare(std::move(genericParamSymbol))) {
            context.reportError(classDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
        }
    }

    for (const auto& member : classDecl.getMemberDeclarations()) {
        if (auto* varDecl = dynamic_cast<VnlcValueDeclarationNode*>(member.get())) {
            checkValueDeclaration(*varDecl);
        } else if (auto* funcDecl = dynamic_cast<VnlcFunctionDeclarationNode*>(member.get())) {
            checkFunctionDeclaration(*funcDecl);
        } else {
            context.reportError(*member, "Invalid class member declaration");
        }
    }

    context.popScope();

    if (context.getErrors().size() == errorCount && isActiveTypeDeclaration(classDecl, classDecl.getName().getIdentifierString())) {
        registerLocalCustomizedType(classDecl, classDecl.getName().getIdentifierString(), VnlcCustomizedTypeKind::CLASS, config);
    }
}

void VnlcSemanticAnalyzer::checkInterfaceDeclaration(const VnlcInterfaceDeclarationNode& interfaceDecl, const VnlcConfig& config, VnlcMetadataInfo metadataInfo) {
    const std::size_t errorCount = context.getErrors().size();
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::INTERFACE, &context.currentScope(), &interfaceDecl));

    for (const auto& member : interfaceDecl.getMethodDeclarations()) {
        VnlcSymbol memberSymbol(
            VnlcSymbolKind::METHOD,

            static_cast<VnlcSymbolAccessModifier>(member->getAccessModifier()),
            member->getName().getIdentifierString(),
            member.get()
        );
        if (!context.currentScope().declare(std::move(memberSymbol))) {
            context.reportError(*member, fmt::format("Redeclaration of interface method '{}'", member->getName().getIdentifierString()));
        }
    }

    for (const auto& genericParamName : interfaceDecl.getGenericParameterNames()) {
        VnlcSymbol genericParamSymbol(VnlcSymbolKind::GENERIC_PARAMETER, VnlcSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &interfaceDecl);
        if (!context.currentScope().declare(std::move(genericParamSymbol))) {
            context.reportError(interfaceDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
        }
    }

    for (const auto& member : interfaceDecl.getMethodDeclarations()) {
        if (auto* funcDecl = dynamic_cast<VnlcFunctionDeclarationNode*>(member.get())) {
            checkFunctionDeclaration(*funcDecl);
        } else {
            context.reportError(*member, "Invalid interface member declaration");
        }
    }

    context.popScope();

    if (context.getErrors().size() == errorCount && isActiveTypeDeclaration(interfaceDecl, interfaceDecl.getName().getIdentifierString())) {
        registerLocalCustomizedType(interfaceDecl, interfaceDecl.getName().getIdentifierString(), VnlcCustomizedTypeKind::INTERFACE, config);
    }
}

void VnlcSemanticAnalyzer::checkEnumDeclaration(const VnlcEnumDeclarationNode& enumDecl, const VnlcConfig& config, VnlcMetadataInfo metadataInfo) {
    const std::size_t errorCount = context.getErrors().size();
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::ENUM, &context.currentScope(), &enumDecl));

    for (const auto& member : enumDecl.getMemberDeclarations()) {
        VnlcSymbol memberSymbol(VnlcSymbolKind::ENUM_MEMBER, VnlcSymbolAccessModifier::PUBLIC, member->getName().getIdentifierString(), member.get());
        if (!context.currentScope().declare(std::move(memberSymbol))) {
            context.reportError(*member, fmt::format("Redeclaration of enum member '{}'", member->getName().getIdentifierString()));
        }
    }

    for (const auto& genericParamName : enumDecl.getGenericParameterNames()) {
        VnlcSymbol genericParamSymbol(VnlcSymbolKind::GENERIC_PARAMETER, VnlcSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &enumDecl);
        if (!context.currentScope().declare(std::move(genericParamSymbol))) {
            context.reportError(enumDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
        }
    }

    for (const auto& member : enumDecl.getMemberDeclarations()) {
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::ENUM_MEMBER, &context.currentScope(), member.get()));

        for (auto& associatedValue : member->getAssociatedValues()) {
            VnlcSymbol associatedValueSymbol(
                VnlcSymbolKind::PROPERTY,

                VnlcSymbolAccessModifier::PUBLIC,
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
        registerLocalCustomizedType(enumDecl, enumDecl.getName().getIdentifierString(), VnlcCustomizedTypeKind::ENUM, config);
    }
}

void VnlcSemanticAnalyzer::checkTypeAliasDeclaration(const VnlcTypeAliasDeclarationNode& typeAliasDecl, const VnlcConfig& config, VnlcMetadataInfo metadataInfo) {
    const std::size_t errorCount = context.getErrors().size();
    context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::TYPE_ALIAS, &context.currentScope(), &typeAliasDecl));

    for (const auto& genericParamName : typeAliasDecl.getGenericParameterNames()) {
        VnlcSymbol genericParamSymbol(VnlcSymbolKind::GENERIC_PARAMETER, VnlcSymbolAccessModifier::PUBLIC, genericParamName->getIdentifierString(), &typeAliasDecl);
        if (!context.currentScope().declare(std::move(genericParamSymbol))) {
            context.reportError(typeAliasDecl, fmt::format("Redeclaration of generic parameter '{}'", genericParamName->getIdentifierString()));
        }
    }

    checkType(typeAliasDecl.getOriginalType());

    context.popScope();

    if (context.getErrors().size() == errorCount && isActiveTypeDeclaration(typeAliasDecl, typeAliasDecl.getAliasName().getIdentifierString())) {
        registerLocalCustomizedType(typeAliasDecl, typeAliasDecl.getAliasName().getIdentifierString(), VnlcCustomizedTypeKind::TYPE_ALIAS, config);
    }
}

void VnlcSemanticAnalyzer::checkStatement(const VnlcStatementNode& statement) {
    if (auto* stmt = dynamic_cast<const VnlcBlockStatementNode*>(&statement)) {
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::BLOCK, &context.currentScope(), stmt));

        for (const auto& child : stmt->getStatements()) {
            checkStatement(*child);
        }

        context.popScope();
    } else if (auto* stmt = dynamic_cast<const VnlcBreakStatementNode*>(&statement)) {
        if (!context.currentLoop()) {
            context.reportError(*stmt, "Break statement not within a loop");
        }

        const auto& label = stmt->getLabel();
        if (label.has_value()) {
            const auto& labelSymbol = context.currentScope().lookup(label.value()->getIdentifierString());
            if (!labelSymbol.has_value()) {
                context.reportError(*stmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
            } else if (labelSymbol.value()->getKind() != VnlcSymbolKind::LOOP_LABEL) {
                context.reportError(*stmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
            }
        }
    } else if (auto* stmt = dynamic_cast<const VnlcContinueStatementNode*>(&statement)) {
        if (!context.currentLoop()) {
            context.reportError(*stmt, "Continue statement not within a loop");
        }

        const auto& label = stmt->getLabel();
        if (label.has_value()) {
            const auto& labelSymbol = context.currentScope().lookup(label.value()->getIdentifierString());
            if (!labelSymbol.has_value()) {
                context.reportError(*stmt, fmt::format("Label '{}' does not exist", label.value()->getIdentifierString()));
            } else if (labelSymbol.value()->getKind() != VnlcSymbolKind::LOOP_LABEL) {
                context.reportError(*stmt, fmt::format("Identifier '{}' is not a loop label", label.value()->getIdentifierString()));
            }
        }
    } else if (auto* stmt = dynamic_cast<const VnlcExpressionStatementNode*>(&statement)) {
        checkExpression(stmt->getExpression());
    } else if (auto* stmt = dynamic_cast<const VnlcForStatementNode*>(&statement)) {
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::LOOP, &context.currentScope(), stmt));

        const auto& label = stmt->getLabel();
        if (label.has_value()) {
            VnlcSymbol labelSymbol(VnlcSymbolKind::LOOP_LABEL, VnlcSymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), stmt);

            if (!context.currentScope().declare(std::move(labelSymbol))) {
                context.reportError(*stmt, fmt::format("Redeclaration of identifier '{}'", label.value()->getIdentifierString()));
            }
        }

        checkValueDeclaration(stmt->getLoopVariable());
        checkExpression(stmt->getIterableExpression());
        checkStatement(stmt->getBody());

        context.popScope();
    } else if (auto* stmt = dynamic_cast<const VnlcIfStatementNode*>(&statement)) {
        checkExpression(stmt->getCondition());
        checkStatement(stmt->getThenBranch());
        if (stmt->getElseBranch().has_value()) {
            checkStatement(*stmt->getElseBranch().value());
        }
    } else if (auto* stmt = dynamic_cast<const VnlcReturnStatementNode*>(&statement)) {
        if (stmt->getReturnValue().has_value()) {
            checkExpression(*stmt->getReturnValue().value());
        }
    } else if (auto* stmt = dynamic_cast<const VnlcSwitchStatementNode*>(&statement)) {
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::SWITCH, &context.currentScope(), stmt));

        checkExpression(stmt->getSwitchExpression());

        if (stmt->getSwitchType() == VnlcSwitchStatementType::LITERAL_MATCH) {
            for (const auto& item : stmt->getLiteralMatchItems()) {
                checkStatement(*item.body);
            }
        } else if (stmt->getSwitchType() == VnlcSwitchStatementType::TYPE_MATCH) {
            for (const auto& item : stmt->getTypeMatchItems()) {
                checkType(*item.type);
                checkStatement(*item.body);
            }
        }

        if (stmt->getDefaultCaseBody().has_value()) {
            checkStatement(*stmt->getDefaultCaseBody().value());
        }

        context.popScope();
    } else if (auto* stmt = dynamic_cast<const VnlcVariableDeclarationStatementNode*>(&statement)) {
        checkValueDeclaration(stmt->getVariableDeclaration());
    } else if (auto* stmt = dynamic_cast<const VnlcWhileStatementNode*>(&statement)) {
        context.pushScope(std::make_unique<VnlcScope>(VnlcScopeKind::LOOP, &context.currentScope(), stmt));

        const auto& label = stmt->getLabel();
        if (label.has_value()) {
            VnlcSymbol labelSymbol(VnlcSymbolKind::LOOP_LABEL, VnlcSymbolAccessModifier::PUBLIC, label.value()->getIdentifierString(), stmt);

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

void VnlcSemanticAnalyzer::checkExpression(const VnlcExpressionNode& expression) {
    // TODO: Implement expression checking process
}

void VnlcSemanticAnalyzer::checkType(const VnlcTypeNode& type) {
    // TODO: Implement type checking process
}

VnlcTypeInferenceResult VnlcSemanticAnalyzer::inferExpressionType(const VnlcExpressionNode& expression) {
    // TODO: Implement expression type inference

    return VnlcTypeInferenceResult::failed();
}

VnlcTypeInferenceResult VnlcSemanticAnalyzer::inferFunctionReturnType(const VnlcFunctionDeclarationNode& funcDecl) {
    // TODO: Implement function return type inference

    return VnlcTypeInferenceResult::failed();
}

VnlcSemanticAnalysisResult VnlcSemanticAnalyzer::analyze(const VnlcConfig& config) {
    checkModule(module, config);

    auto diagnostics = context.takeDiagnostics();
    auto customizedTypes = context.takeCustomizedTypeRegistry();
    auto semanticTypes = context.takeSemanticTypeMap();
    auto inferredValueTypes = context.takeInferredValueTypeMap();
    auto inferredFunctionReturnTypes = context.takeInferredFunctionReturnTypeMap();
    auto inferredExpressionTypes = context.takeInferredExpressionTypeMap();
    auto importedPackages = context.takeImportedPackages();
    auto importedBindings = context.takeImportedBindings();
    return VnlcSemanticAnalysisResult(
        std::move(std::get<0>(diagnostics)),
        std::move(std::get<1>(diagnostics)),
        std::move(std::get<2>(diagnostics)),
        std::move(customizedTypes),
        std::move(semanticTypes),
        std::move(inferredValueTypes),
        std::move(inferredFunctionReturnTypes),
        std::move(inferredExpressionTypes),
        std::move(importedPackages),
        std::move(importedBindings)
    );
}
