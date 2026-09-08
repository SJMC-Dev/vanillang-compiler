#ifndef VNLC_CLASS_DECLARATION_NODE_HPP
#define VNLC_CLASS_DECLARATION_NODE_HPP

#include "ast/declaration/VnlcTypeDeclarationNode.hpp"
#include "ast/identifier/VnlcIdentifierNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <memory>
#include <optional>
#include <vector>

class VnlcClassDeclarationNode : public VnlcTypeDeclarationNode {
private:
    VnlcClassDeclarationNode() = delete;

    bool final;
    std::unique_ptr<VnlcIdentifierNode> name;
    std::optional<std::unique_ptr<VnlcTypeNode>> baseClass;           // nullopt if no base class
    std::vector<std::unique_ptr<VnlcTypeNode>> implementedInterfaces; // empty if no implemented interfaces
    std::vector<std::unique_ptr<VnlcIdentifierNode>> genericParameterNames;
    std::vector<std::unique_ptr<VnlcDeclarationNode>> memberDeclarations;

public:
    VnlcClassDeclarationNode(
        bool final,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& baseClass,
        std::vector<std::unique_ptr<VnlcTypeNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<VnlcDeclarationNode>>&& memberDeclarations,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    VnlcClassDeclarationNode(
        bool final,
        std::unique_ptr<VnlcIdentifierNode>&& name,
        std::optional<std::unique_ptr<VnlcTypeNode>>&& baseClass,
        std::vector<std::unique_ptr<VnlcTypeNode>>&& implementedInterfaces,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& genericParameterNames,
        std::vector<std::unique_ptr<VnlcDeclarationNode>>&& memberDeclarations,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken,
        std::vector<VnlcDeclarationItem::MetadataTerm>&& metadataTerms
    ) noexcept;

    [[nodiscard]] const bool isFinal() const noexcept;
    [[nodiscard]] const VnlcIdentifierNode& getName() const noexcept;
    [[nodiscard]] const std::optional<std::unique_ptr<VnlcTypeNode>>& getBaseClass() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcTypeNode>>& getImplementedInterfaces() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcIdentifierNode>>& getGenericParameterNames() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcDeclarationNode>>& getMemberDeclarations() const noexcept;
};

#endif // VNLC_CLASS_DECLARATION_NODE_HPP