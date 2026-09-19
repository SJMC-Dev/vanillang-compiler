#include "TypeNode.hpp"

namespace vnlc {
    TypeNode::TypeNode(
        bool questionMarkSuffix,
        std::vector<std::unique_ptr<IdentifierNode>>&& nameParts,
        std::vector<std::unique_ptr<TypeNode>>&& genericArguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : AstNode(firstToken, lastToken),
          questionMarkSuffix(questionMarkSuffix),
          nameParts(std::move(nameParts)),
          genericArguments(std::move(genericArguments)) {}

    const bool TypeNode::hasQuestionMarkSuffix() const noexcept {
        return questionMarkSuffix;
    }

    const std::vector<std::unique_ptr<IdentifierNode>>& TypeNode::getNameParts() const noexcept {
        return nameParts;
    }

    const std::vector<std::unique_ptr<TypeNode>>& TypeNode::getGenericArguments() const noexcept {
        return genericArguments;
    }
} // namespace vnlc
