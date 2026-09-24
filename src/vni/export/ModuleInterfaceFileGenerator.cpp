#include "ModuleInterfaceFileGenerator.hpp"
#include "ast/declaration/ClassDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "nlohmann/json_fwd.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>

namespace vnlc {
    ModuleInterfaceFileGenerator::ModuleInterfaceFileGenerator(
        std::vector<const DeclarationNode*>&& declarationNodes,
        std::vector<std::string>&& importedAliases,
        const Config& config,
        const SemanticAnalysisResult& semantic
    ) noexcept
        : declarationNodes(std::move(declarationNodes)),
          importedAliases(std::move(importedAliases)),
          config(config),
          semantic(semantic) {}

    nlohmann::json ModuleInterfaceFileGenerator::stringifyMetadata(const std::vector<DeclarationItem::MetadataTerm>& metadataTerms) {
        nlohmann::json metadataObj = nlohmann::json::object();

        for (const auto& metadataTerm : metadataTerms) {
            metadataObj.emplace(metadataTerm.key->getIdentifierString(), metadataTerm.value);
        }

        return metadataObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyGenericParameters(const std::vector<std::unique_ptr<IdentifierNode>>& genericParameterNames) {
        nlohmann::json genericParametersObj = nlohmann::json::array();

        for (const auto& genericParam : genericParameterNames) {
            genericParametersObj.push_back(genericParam->getIdentifierString());
        }

        return genericParametersObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyParameter(const ValueDeclarationNode* parameter) {
        nlohmann::json parameterObj = nlohmann::json::object();
        parameterObj.emplace("category", "parameter");

        const auto& typeNode = parameter->getType();
        if (typeNode.has_value()) {
            const auto* type = semantic.getTypeByTypeReferenceNode(typeNode.value().get());
            if (type != nullptr) {
                parameterObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(parameter);
            if (inferredType != nullptr) {
                parameterObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        return parameterObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyVariable(const ValueDeclarationNode* variable) {
        nlohmann::json variableObj = nlohmann::json::object();
        variableObj.emplace("category", "let");

        const auto& metadata = variable->getMetadataTerms();
        if (!metadata.empty()) {
            variableObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto& typeNode = variable->getType();
        if (typeNode.has_value()) {
            const auto* type = semantic.getTypeByTypeReferenceNode(typeNode.value().get());
            if (type != nullptr) {
                variableObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(variable);
            if (inferredType != nullptr) {
                variableObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        return variableObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyFunction(const FunctionDeclarationNode* function) {
        nlohmann::json functionObj = nlohmann::json::object();
        functionObj.emplace("category", "func");

        const auto& metadata = function->getMetadataTerms();

        if (!metadata.empty()) {
            functionObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto& returnTypeReferenceNode = function->getReturnType();
        if (returnTypeReferenceNode.has_value()) {
            const auto* type = semantic.getTypeByTypeReferenceNode(returnTypeReferenceNode.value().get());
            if (type != nullptr) {
                functionObj.emplace("returnType", type->getFullTypeName());
            }
        } else {
            const auto* inferredReturnType = semantic.getInferredFunctionReturnType(function);
            if (inferredReturnType != nullptr) {
                functionObj.emplace("returnType", inferredReturnType->getFullTypeName());
            }
        }

        nlohmann::json parametersObj = nlohmann::json::object();
        for (const auto& parameter : function->getParameters()) {
            parametersObj.emplace(parameter->getName().getIdentifierString(), stringifyParameter(parameter.get()));
        }

        functionObj.emplace("parameters", parametersObj);
        functionObj.emplace("native", function->getKind() == FunctionDeclarationKind::Kind::NATIVE);

        return functionObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyClass(const ClassDeclarationNode* classNode) {
        nlohmann::json classObj = nlohmann::json::object();
        classObj.emplace("category", "class");

        const auto& metadata = classNode->getMetadataTerms();
        if (!metadata.empty()) {
            classObj.emplace("metadata", stringifyMetadata(metadata));
        }

        std::optional<std::string> baseClassName;
        if (classNode->getBaseClass().has_value()) {
            const auto& baseClassTypeReferenceNode = classNode->getBaseClass().value().get();
            const auto* type = semantic.getTypeByTypeReferenceNode(baseClassTypeReferenceNode);
            if (type != nullptr) {
                baseClassName = type->getFullTypeName();
            }
        }
        classObj.emplace("baseClass", baseClassName);

        std::vector<std::string> implementedInterfaceNames;
        for (const auto& interfaceTypeReferenceNode : classNode->getImplementedInterfaces()) {
            const auto* type = semantic.getTypeByTypeReferenceNode(interfaceTypeReferenceNode.get());
            if (type != nullptr) {
                implementedInterfaceNames.emplace_back(type->getFullTypeName());
            }
        }
        classObj.emplace("implementedInterfaces", implementedInterfaceNames);

        classObj.emplace("final", classNode->isFinal());

        classObj.emplace("genericParameters", stringifyGenericParameters(classNode->getGenericParameterNames()));

        nlohmann::json propertiesObj = nlohmann::json::object();
        nlohmann::json methodsObj = nlohmann::json::object();

        for (const auto& memberDeclaration : classNode->getMemberDeclarations()) {
            if (const auto* property = dynamic_cast<const ValueDeclarationNode*>(memberDeclaration.get())) {
                propertiesObj.emplace(property->getName().getIdentifierString(), stringifyProperty(property));
            } else if (const auto* method = dynamic_cast<const FunctionDeclarationNode*>(memberDeclaration.get())) {
                methodsObj.emplace(method->getName().getIdentifierString(), stringifyMethod(method));
            }
        }

        classObj.emplace("properties", propertiesObj);
        classObj.emplace("methods", methodsObj);

        return classObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyInterface(const InterfaceDeclarationNode* interfaceNode) {
        nlohmann::json interfaceObj = nlohmann::json::object();
        interfaceObj.emplace("category", "interface");

        const auto& metadata = interfaceNode->getMetadataTerms();
        if (!metadata.empty()) {
            interfaceObj.emplace("metadata", stringifyMetadata(metadata));
        }

        interfaceObj.emplace("genericParameters", stringifyGenericParameters(interfaceNode->getGenericParameterNames()));

        nlohmann::json methodsObj = nlohmann::json::object();
        for (const auto& method : interfaceNode->getMethodDeclarations()) {
            nlohmann::json methodObj = stringifyMethod(method.get());
            methodsObj.emplace(method->getName().getIdentifierString(), methodObj);
        }
        interfaceObj.emplace("methods", methodsObj);

        return interfaceObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyEnum(const EnumDeclarationNode* enumNode) {
        nlohmann::json enumObj = nlohmann::json::object();
        enumObj.emplace("category", "enum");

        enumObj.emplace("genericParameters", stringifyGenericParameters(enumNode->getGenericParameterNames()));

        nlohmann::json membersObj = nlohmann::json::object();
        for (const auto& member : enumNode->getMemberDeclarations()) {
            nlohmann::json memberObj = stringifyEnumMember(member.get());
            membersObj.emplace(member->getName().getIdentifierString(), memberObj);
        }
        enumObj.emplace("members", membersObj);

        return enumObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyEnumMember(const EnumMemberDeclarationNode* enumMemberNode) {
        nlohmann::json memberObj = nlohmann::json::object();
        memberObj.emplace("category", "enummember");

        nlohmann::json associatedValuesObj = nlohmann::json::object();
        for (const auto& associatedValue : enumMemberNode->getAssociatedValues()) {
            nlohmann::json associatedValueObj = stringifyEnumValue(associatedValue.get());
            associatedValuesObj.emplace(associatedValue->getName().getIdentifierString(), associatedValueObj);
        }
        memberObj.emplace("associatedValues", associatedValuesObj);

        return memberObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyEnumValue(const ValueDeclarationNode* enumValue) {
        nlohmann::json enumValueObj = nlohmann::json::object();
        enumValueObj.emplace("category", "enumvalue");

        const auto& typeNode = enumValue->getType();
        if (typeNode.has_value()) {
            const auto* type = semantic.getTypeByTypeReferenceNode(typeNode.value().get());
            if (type != nullptr) {
                enumValueObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(enumValue);
            if (inferredType != nullptr) {
                enumValueObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        return enumValueObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyTypeAlias(const TypeAliasDeclarationNode* typeAliasNode) {
        nlohmann::json typeAliasObj = nlohmann::json::object();
        typeAliasObj.emplace("category", "typealias");

        typeAliasObj.emplace("genericParameters", stringifyGenericParameters(typeAliasNode->getGenericParameterNames()));
        const auto* type = semantic.getTypeByTypeReferenceNode(&typeAliasNode->getOriginalType());
        if (type != nullptr) {
            typeAliasObj.emplace("originalType", type->getFullTypeName());
        }

        return typeAliasObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyProperty(const ValueDeclarationNode* property) {
        nlohmann::json propertyObj = nlohmann::json::object();
        propertyObj.emplace("category", "property");

        const auto& metadata = property->getMetadataTerms();
        if (!metadata.empty()) {
            propertyObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto& typeNode = property->getType();
        if (typeNode.has_value()) {
            const auto* type = semantic.getTypeByTypeReferenceNode(typeNode.value().get());
            if (type != nullptr) {
                propertyObj.emplace("type", type->getFullTypeName());
            }
        } else {
            const auto* inferredType = semantic.getInferredValueType(property);
            if (inferredType != nullptr) {
                propertyObj.emplace("type", inferredType->getFullTypeName());
            }
        }

        propertyObj.emplace("static", property->getKind() == ValueDeclarationKind::Kind::STATIC_PROPERTY);

        std::string accessModifier;
        switch (property->getAccessModifier()) {
            case ValueDeclarationKind::AccessModifier::PUBLIC:
                accessModifier = "public";
                break;
            case ValueDeclarationKind::AccessModifier::PROTECTED:
                accessModifier = "protected";
                break;
            case ValueDeclarationKind::AccessModifier::PRIVATE:
                accessModifier = "private";
                break;
        }
        propertyObj.emplace("accessModifier", accessModifier);

        return propertyObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyMethod(const FunctionDeclarationNode* method) {
        nlohmann::json methodObj = nlohmann::json::object();
        methodObj.emplace("category", "method");

        const auto& metadata = method->getMetadataTerms();

        if (!metadata.empty()) {
            methodObj.emplace("metadata", stringifyMetadata(metadata));
        }

        const auto& returnTypeReferenceNode = method->getReturnType();
        if (returnTypeReferenceNode.has_value()) {
            const auto* type = semantic.getTypeByTypeReferenceNode(returnTypeReferenceNode.value().get());
            if (type != nullptr) {
                methodObj.emplace("returnType", type->getFullTypeName());
            }
        } else {
            const auto* inferredReturnType = semantic.getInferredFunctionReturnType(method);
            if (inferredReturnType != nullptr) {
                methodObj.emplace("returnType", inferredReturnType->getFullTypeName());
            }
        }

        nlohmann::json parametersObj = nlohmann::json::object();
        for (const auto& parameter : method->getParameters()) {
            parametersObj.emplace(parameter->getName().getIdentifierString(), stringifyParameter(parameter.get()));
        }

        methodObj.emplace("parameters", parametersObj);
        methodObj.emplace("native", method->getKind() == FunctionDeclarationKind::Kind::NATIVE);
        methodObj.emplace("static", method->getBinding() == FunctionDeclarationKind::Binding::STATIC);

        std::string accessModifier;
        switch (method->getAccessModifier()) {
            case FunctionDeclarationKind::AccessModifier::PUBLIC:
                accessModifier = "public";
                break;
            case FunctionDeclarationKind::AccessModifier::PROTECTED:
                accessModifier = "protected";
                break;
            case FunctionDeclarationKind::AccessModifier::PRIVATE:
                accessModifier = "private";
                break;
        }
        methodObj.emplace("accessModifier", accessModifier);

        return methodObj;
    }

    nlohmann::json ModuleInterfaceFileGenerator::stringifyImported(std::string_view importedAlias) {
        nlohmann::json importedObj = nlohmann::json::object();
        importedObj.emplace("category", "imported");
        importedObj.emplace("source", importedAlias);

        return importedObj;
    }

    void ModuleInterfaceFileGenerator::generate() {
        if (!config.moduleInterfaceOutputDirectory.has_value()) {
            return;
        }

        nlohmann::json moduleObj = nlohmann::json::object();

        for (const auto* declarationNode : declarationNodes) {
            if (const auto* variable = dynamic_cast<const ValueDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(variable->getName().getIdentifierString(), stringifyVariable(variable));
            } else if (const auto* function = dynamic_cast<const FunctionDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(function->getName().getIdentifierString(), stringifyFunction(function));
            } else if (const auto* classNode = dynamic_cast<const ClassDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(classNode->getName().getIdentifierString(), stringifyClass(classNode));
            } else if (const auto* interfaceNode = dynamic_cast<const InterfaceDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(interfaceNode->getName().getIdentifierString(), stringifyInterface(interfaceNode));
            } else if (const auto* enumNode = dynamic_cast<const EnumDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(enumNode->getName().getIdentifierString(), stringifyEnum(enumNode));
            } else if (const auto* typeAliasNode = dynamic_cast<const TypeAliasDeclarationNode*>(declarationNode)) {
                moduleObj.emplace(typeAliasNode->getAliasName().getIdentifierString(), stringifyTypeAlias(typeAliasNode));
            } else {
                assert(false && "Unsupported declaration node in module interface file generator");
            }
        }

        for (const auto& importedAlias : importedAliases) {
            moduleObj.emplace(importedAlias, stringifyImported(importedAlias));
        }

        std::filesystem::path relativeInputPath = std::filesystem::relative(config.inputFilePath, config.packageRootPath);
        std::filesystem::path outputPath = config.moduleInterfaceOutputDirectory.value() / config.packageRootPath.filename() / relativeInputPath;
        outputPath.replace_extension(".vni");

        std::filesystem::create_directories(outputPath.parent_path());

        std::ofstream outputFile;
        outputFile.exceptions(std::ios::failbit | std::ios::badbit);
        outputFile.open(outputPath);
        outputFile << moduleObj.dump(4) << '\n';
    }
} // namespace vnlc
