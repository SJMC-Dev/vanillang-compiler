#ifndef VNLC_INTERFACE_DECLARATION_NODE_HPP
#define VNLC_INTERFACE_DECLARATION_NODE_HPP

#include "ast/declaration/FunctionDeclarationNode.hpp"
#include "ast/declaration/TypeDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class InterfaceDeclarationNode : public TypeDeclarationNode {
    private:
        InterfaceDeclarationNode() = delete;

        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> methodDeclarations;

    public:
        InterfaceDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        InterfaceDeclarationNode(
            std::unique_ptr<IdentifierNode>&& name,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::vector<std::unique_ptr<FunctionDeclarationNode>>&& methodDeclarations,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameterNames() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<FunctionDeclarationNode>>& getMethodDeclarations() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_DECLARATION_NODE_HPP