#ifndef VNLC_TYPE_NODE_HPP
#define VNLC_TYPE_NODE_HPP

#include "ast/AstNode.hpp"
#include "ast/identifier/IdentifierNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class TypeNode : public AstNode {
    private:
        TypeNode() = delete;

        bool questionMarkSuffix; // true if the type has a '?' suffix, false otherwise
        std::vector<std::unique_ptr<IdentifierNode>> nameParts;
        std::vector<std::unique_ptr<TypeNode>> genericArguments; // empty if not a generic type

    public:
        TypeNode(
            bool questionMarkSuffix,
            std::vector<std::unique_ptr<IdentifierNode>>&& nameParts,
            std::vector<std::unique_ptr<TypeNode>>&& genericArguments,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const bool hasQuestionMarkSuffix() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getNameParts() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeNode>>& getGenericArguments() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_NODE_HPP