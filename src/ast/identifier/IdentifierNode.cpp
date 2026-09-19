#include "IdentifierNode.hpp"
#include <string_view>

namespace vnlc {
    IdentifierNode::IdentifierNode(std::string_view identifierString, const Token& firstToken, const Token& lastToken) : AstNode(firstToken, lastToken), identifierString(identifierString) {}

    std::string_view IdentifierNode::getIdentifierString() const {
        return identifierString;
    }
} // namespace vnlc
