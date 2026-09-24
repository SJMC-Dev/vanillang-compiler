#include "TypeReferenceNode.hpp"

namespace vnlc {
    TypeReferenceNode::TypeReferenceNode(bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept
        : AstNode(firstToken, lastToken),
          questionMarkSuffix(questionMarkSuffix) {}

    const bool TypeReferenceNode::hasQuestionMarkSuffix() const noexcept {
        return questionMarkSuffix;
    }
} // namespace vnlc
