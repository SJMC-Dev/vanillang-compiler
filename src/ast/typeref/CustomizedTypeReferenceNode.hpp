#ifndef VNLC_CUSTOMIZED_TYPE_REFERENCE_NODE_HPP
#define VNLC_CUSTOMIZED_TYPE_REFERENCE_NODE_HPP

#include "ast/identifier/IdentifierNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class CustomizedTypeReferenceNode : public TypeReferenceNode {
    private:
        CustomizedTypeReferenceNode() = delete;

        std::vector<std::unique_ptr<IdentifierNode>> nameParts;
        std::vector<std::unique_ptr<TypeReferenceNode>> genericArguments;

    public:
        CustomizedTypeReferenceNode(
            bool questionMarkSuffix,
            std::vector<std::unique_ptr<IdentifierNode>>&& nameParts,
            std::vector<std::unique_ptr<TypeReferenceNode>>&& genericArguments,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] const std::vector<std::unique_ptr<IdentifierNode>>& getNameParts() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeReferenceNode>>& getGenericArguments() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CUSTOMIZED_TYPE_REFERENCE_NODE_HPP
