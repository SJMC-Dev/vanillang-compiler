#ifndef VNLC_IDENTIFIER_NODE_HPP
#define VNLC_IDENTIFIER_NODE_HPP

#include "ast/VnlcAstNode.hpp"
#include <string>
#include <string_view>

class VnlcIdentifierNode : public VnlcAstNode {
private:
    VnlcIdentifierNode() = delete;

    std::string identifierString;

public:
    VnlcIdentifierNode(std::string_view identifierString, const VnlcToken& firstToken, const VnlcToken& lastToken);

    [[nodiscard]] std::string_view getIdentifierString() const;
};

#endif // VNLC_IDENTIFIER_NODE_HPP