#ifndef VNLC_VALUE_DECLARATION_NODE_HPP
#define VNLC_VALUE_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationType.hpp"
#include "ast/expression/ExpressionNode.hpp"
#include "ast/type/TypeNode.hpp"
#include <memory>

namespace vnlc {
    class ValueDeclarationNode : public DeclarationNode {
    private:
        ValueDeclarationNode() = delete;

        ValueDeclarationType::Kind kind;
        ValueDeclarationType::Context context;
        ValueDeclarationType::AccessModifier accessModifier;
        std::unique_ptr<IdentifierNode> name;
        std::optional<std::unique_ptr<TypeNode>> type;
        std::optional<std::unique_ptr<ExpressionNode>> initializer;

    public:
        ValueDeclarationNode(
            ValueDeclarationType::Kind kind,
            ValueDeclarationType::Context context,
            ValueDeclarationType::AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::optional<std::unique_ptr<TypeNode>>&& type,
            std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        ValueDeclarationNode(
            ValueDeclarationType::Kind kind,
            ValueDeclarationType::Context context,
            ValueDeclarationType::AccessModifier accessModifier,
            std::unique_ptr<IdentifierNode>&& name,
            std::optional<std::unique_ptr<TypeNode>>&& type,
            std::optional<std::unique_ptr<ExpressionNode>>&& initializer,
            const Token& firstToken,
            const Token& lastToken,
            std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
        ) noexcept;

        [[nodiscard]] const ValueDeclarationType::Kind getKind() const noexcept;
        [[nodiscard]] const ValueDeclarationType::Context getContext() const noexcept;
        [[nodiscard]] const ValueDeclarationType::AccessModifier getAccessModifier() const noexcept;
        [[nodiscard]] const IdentifierNode& getName() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<TypeNode>>& getType() const noexcept;
        [[nodiscard]] const std::optional<std::unique_ptr<ExpressionNode>>& getInitializer() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_VALUE_DECLARATION_NODE_HPP