#include "PrimitiveTypeReferenceNode.hpp"

namespace vnlc {
    PrimitiveTypeReferenceNode::PrimitiveTypeReferenceNode(PrimitiveTypeReferenceKind kind, bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept
        : TypeReferenceNode(questionMarkSuffix, firstToken, lastToken),
          kind(kind) {}

    const PrimitiveTypeReferenceKind PrimitiveTypeReferenceNode::getKind() const noexcept {
        return kind;
    }

    std::string_view PrimitiveTypeReferenceNode::getPrimitiveTypeName() const noexcept {
        switch (kind) {
            case PrimitiveTypeReferenceKind::BYTE:
                return "byte";
            case PrimitiveTypeReferenceKind::SHORT:
                return "short";
            case PrimitiveTypeReferenceKind::INT:
                return "int";
            case PrimitiveTypeReferenceKind::LONG:
                return "long";
            case PrimitiveTypeReferenceKind::FLOAT:
                return "float";
            case PrimitiveTypeReferenceKind::DOUBLE:
                return "double";
            case PrimitiveTypeReferenceKind::BOOLEAN:
                return "bool";
            case PrimitiveTypeReferenceKind::STRING:
                return "string";
        }
        return {};
    }
} // namespace vnlc
