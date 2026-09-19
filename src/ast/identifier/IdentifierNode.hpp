#ifndef VNLC_IDENTIFIER_NODE_HPP
#define VNLC_IDENTIFIER_NODE_HPP

#include "ast/AstNode.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class IdentifierNode : public AstNode {
    private:
        IdentifierNode() = delete;

        std::string identifierString;

    public:
        IdentifierNode(std::string_view identifierString, const Token& firstToken, const Token& lastToken);

        [[nodiscard]] std::string_view getIdentifierString() const;
    };
} // namespace vnlc

#endif // VNLC_IDENTIFIER_NODE_HPP