#ifndef VNLC_TYPE_REFERENCE_NODE_HPP
#define VNLC_TYPE_REFERENCE_NODE_HPP

#include "ast/AstNode.hpp"

namespace vnlc {
    class TypeReferenceNode : public AstNode {
    private:
        TypeReferenceNode() = delete;

        bool questionMarkSuffix;

    protected:
        TypeReferenceNode(bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept;

    public:
        [[nodiscard]] const bool hasQuestionMarkSuffix() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_REFERENCE_NODE_HPP
