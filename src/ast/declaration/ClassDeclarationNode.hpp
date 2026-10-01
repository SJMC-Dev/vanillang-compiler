#ifndef VNLC_CLASS_DECLARATION_NODE_HPP
#define VNLC_CLASS_DECLARATION_NODE_HPP

#include "ast/declaration/ConstructorDeclarationNode.hpp"
#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/OperatorDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace vnlc {
    class ClassDeclarationNode : public TypeDeclarationNode {
    private:
        ClassDeclarationNode() = delete;

        bool final;
        std::unique_ptr<IdentifierNode> name;
        std::optional<std::unique_ptr<TypeReferenceNode>> baseClass;           // nullopt if no base class
        std::vector<std::unique_ptr<TypeReferenceNode>> implementedInterfaces; // empty if no implemented interfaces
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::vector<std::unique_ptr<ValueDeclarationNode>> propertyDeclarations; // instance or static properties only
        std::vector<std::unique_ptr<FunctionDeclarationNode>> methodDeclarations;
        std::vector<std::unique_ptr<ConstructorDeclarationNode>> constructorDeclarations;
        std::vector<std::unique_ptr<OperatorDeclarationNode>> operatorDeclarations;

    public:
        ClassDeclarationNode(
            bool final,
            std::unique_ptr<IdentifierNode>&& name,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& baseClass,
            std::vector<std::unique_ptr<TypeReferenceNode>>&& implementedInterfaces,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& propertyDeclarations,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
            std::vector<std::unique_ptr<ConstructorDeclarationNode>>&& constructorDeclarations,
            std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operatorDeclarations,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        ClassDeclarationNode(
            bool final,
            std::unique_ptr<IdentifierNode>&& name,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& baseClass,
            std::vector<std::unique_ptr<TypeReferenceNode>>&& implementedInterfaces,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::vector<std::unique_ptr<ValueDeclarationNode>>&& propertyDeclarations,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
            std::vector<std::unique_ptr<ConstructorDeclarationNode>>&& constructorDeclarations,
            std::vector<std::unique_ptr<OperatorDeclarationNode>>&& operatorDeclarations,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const bool isFinal() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<TypeReferenceNode>>& getBaseClass() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeReferenceNode>>& getImplementedInterfaces() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameterNames() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ValueDeclarationNode>>& getPropertyDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getMethodDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ConstructorDeclarationNode>>& getConstructorDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<OperatorDeclarationNode>>& getOperatorDeclarations() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CLASS_DECLARATION_NODE_HPP
