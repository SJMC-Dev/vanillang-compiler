#ifndef VNLC_VALUE_DECLARATION_NODE_HPP
#define VNLC_VALUE_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationKind.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>

namespace vnlc {
    class ValueDeclarationNode : public DeclarationNode {
    private:
        ValueDeclarationNode() = delete;

        ValueDeclarationKind::Kind kind;
        ValueDeclarationKind::Context context;
        ValueDeclarationKind::AccessModifier accessModifier;
        std::unique_ptr<IdentifierNode> name;
        std::optional<std::unique_ptr<TypeReferenceNode>> type;
        std::optional<std::unique_ptr<ExpressionNode>> initializer;

    public:
        ValueDeclarationNode(
            ValueDeclarationKind::Kind kind,
            ValueDeclarationKind::Context context,
            ValueDeclarationKind::AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& type,
            std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        ValueDeclarationNode(
            ValueDeclarationKind::Kind kind,
            ValueDeclarationKind::Context context,
            ValueDeclarationKind::AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::optional<std::unique_ptr<TypeReferenceNode>>&& type,
            std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const ValueDeclarationKind::Kind getKind() const noexcept;
        [[nodiscard]] const ValueDeclarationKind::Context getContext() const noexcept;
        [[nodiscard]] const ValueDeclarationKind::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<TypeReferenceNode>>& getType() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<ExpressionNode>>& getInitializer() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_VALUE_DECLARATION_NODE_HPP