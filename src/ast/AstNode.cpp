#include "AstNode.hpp"

namespace vnlc {
    AstNode::AstNode(const Token& firstToken, const Token& lastToken)
        : offset(firstToken.getOffset()),
          length(lastToken.getOffset() - firstToken.getOffset()),
          line(firstToken.getLine()),
          column(firstToken.getColumn()) {}

    std::pair<unsigned int, unsigned int> AstNode::locate() const noexcept {
        return { line, column };
    }

    unsigned int AstNode::getOffset() const noexcept {
        return offset;
    }

    unsigned int AstNode::getLength() const noexcept {
        return length;
    }

    void AstNode::resetPosition(const Token& firstToken, const Token& lastToken) noexcept {
        offset = firstToken.getOffset();
        length = lastToken.getOffset() - firstToken.getOffset();
        line = firstToken.getLine();
        column = firstToken.getColumn();
    }
} // namespace vnlc
