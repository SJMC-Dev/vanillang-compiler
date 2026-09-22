#include "AstNode.hpp"

namespace vnlc {
    AstNode::AstNode(const Token& firstToken, const Token& lastToken)
        : offset(firstToken.getOffset()),
          length(lastToken.getOffset() - firstToken.getOffset()),
          line(firstToken.getLine()),
          column(firstToken.getColumn()) {}

    std::pair<std::size_t, std::size_t> AstNode::locate() const noexcept {
        return { line, column };
    }

    std::size_t AstNode::getOffset() const noexcept {
        return offset;
    }

    std::size_t AstNode::getLength() const noexcept {
        return length;
    }

    void AstNode::resetPosition(const Token& firstToken, const Token& lastToken) noexcept {
        offset = firstToken.getOffset();
        length = lastToken.getOffset() - firstToken.getOffset();
        line = firstToken.getLine();
        column = firstToken.getColumn();
    }
} // namespace vnlc
