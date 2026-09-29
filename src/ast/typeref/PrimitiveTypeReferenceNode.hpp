#ifndef VNLC_PRIMITIVE_TYPE_REFERENCE_NODE_HPP
#define VNLC_PRIMITIVE_TYPE_REFERENCE_NODE_HPP

#include "ast/typeref/PrimitiveTypeReferenceKind.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <string_view>

namespace vnlc {
    class PrimitiveTypeReferenceNode : public TypeReferenceNode {
    private:
        PrimitiveTypeReferenceNode() = delete;

        PrimitiveTypeReferenceKind kind;

    public:
        PrimitiveTypeReferenceNode(PrimitiveTypeReferenceKind kind, bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const PrimitiveTypeReferenceKind getKind() const noexcept;
        [[nodiscard]] std::string_view getPrimitiveTypeName() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_PRIMITIVE_TYPE_REFERENCE_NODE_HPP
