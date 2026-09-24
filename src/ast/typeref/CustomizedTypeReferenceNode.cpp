#include "CustomizedTypeReferenceNode.hpp"

namespace vnlc {
    CustomizedTypeReferenceNode::CustomizedTypeReferenceNode(
        bool questionMarkSuffix,
        std::vector<std::unique_ptr<IdentifierNode>>&& nameParts,
        std::vector<std::unique_ptr<TypeReferenceNode>>&& genericArguments,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeReferenceNode(questionMarkSuffix, firstToken, lastToken),
          nameParts(std::move(nameParts)),
          genericArguments(std::move(genericArguments)) {}

    const std::vector<std::unique_ptr<IdentifierNode>>& CustomizedTypeReferenceNode::getNameParts() const noexcept {
        return nameParts;
    }

    const std::vector<std::unique_ptr<TypeReferenceNode>>& CustomizedTypeReferenceNode::getGenericArguments() const noexcept {
        return genericArguments;
    }
} // namespace vnlc
