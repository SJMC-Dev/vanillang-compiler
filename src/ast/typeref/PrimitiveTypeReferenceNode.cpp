#include "PrimitiveTypeReferenceNode.hpp"

namespace vnlc {
    PrimitiveTypeReferenceNode::PrimitiveTypeReferenceNode(PrimitiveTypeReferenceKind kind, bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept
        : TypeReferenceNode(questionMarkSuffix, firstToken, lastToken),
          kind(kind) {}

    const PrimitiveTypeReferenceKind PrimitiveTypeReferenceNode::getKind() const noexcept {
        return kind;
    }
} // namespace vnlc
