#ifndef VNLC_INTERFACE_DECLARATION_NODE_HPP
#define VNLC_INTERFACE_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include "ast/declaration/VnlcTypeDeclarationNode.hpp"
#include <memory>
#include <vector>

class VnlcInterfaceDeclarationNode : public VnlcTypeDeclarationNode {
private:
    VnlcInterfaceDeclarationNode() = delete;

    std::unique_ptr<VnlcIdentifierNode> name;
    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameterNames;
    std::vector<std::unique_ptr<VnlcFunctionDeclarationNode>> methodDeclarations;

public:
    VnlcInterfaceDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<VnlcFunctionDeclarationNode>>&& methodDeclarations,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcInterfaceDeclarationNode(
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<VnlcFunctionDeclarationNode>>&& methodDeclarations,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcIdentifierNode>>& getGenericParameterNames() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcFunctionDeclarationNode>>& getMethodDeclarations() const noexcept;
};

#endif // VNLC_INTERFACE_DECLARATION_NODE_HPP