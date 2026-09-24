#ifndef VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP

#include "ast/declaration/TypeDeclarationNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class TypeAliasDeclarationNode : public TypeDeclarationNode {
    private:
        TypeAliasDeclarationNode() = delete;

        std::unique_ptr<IdentifierNode> aliasName;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::unique_ptr<TypeReferenceNode> originalType;

    public:
        TypeAliasDeclarationNode(
            std::unique_ptr<IdentifierNode>&& aliasName,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::unique_ptr<TypeReferenceNode>&& originalType,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        TypeAliasDeclarationNode(
            std::unique_ptr<IdentifierNode>&& aliasName,
            std::vector<std::unique_ptr<IdentifierNode>>&& genericParameterNames,
            std::unique_ptr<TypeReferenceNode>&& originalType,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const IdentifierNode& getAliasName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getGenericParameterNames() const noexcept;
        [[nodiscard]] const TypeReferenceNode& getOriginalType() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_ALIAS_DECLARATION_NODE_HPP