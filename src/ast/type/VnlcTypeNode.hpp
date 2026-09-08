#ifndef VNLC_TYPE_NODE_HPP
#define VNLC_TYPE_NODE_HPP

#include "ast/VnlcAstNode.hpp"
#include "ast/identifier/VnlcIdentifierNode.hpp"
#include <memory>
#include <vector>

class VnlcTypeNode : public VnlcAstNode {
private:
    VnlcTypeNode() = delete;

    bool questionMarkSuffix; // true if the type has a '?' suffix, false otherwise
    std::vector<std::unique_ptr<VnlcIdentifierNode>> nameParts;
    std::vector<std::unique_ptr<VnlcTypeNode>> genericArguments; // empty if not a generic type

public:
    VnlcTypeNode(
        bool questionMarkSuffix,
        std::vector<std::unique_ptr<VnlcIdentifierNode>>&& nameParts,
        std::vector<std::unique_ptr<VnlcTypeNode>>&& genericArguments,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] const bool hasQuestionMarkSuffix() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcIdentifierNode>>& getNameParts() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcTypeNode>>& getGenericArguments() const noexcept;
};

#endif // VNLC_TYPE_NODE_HPP