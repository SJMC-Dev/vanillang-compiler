#ifndef VNLC_AST_NODE_HPP
#define VNLC_AST_NODE_HPP

#include "token/VnlcToken.hpp"
#include <utility>

class VnlcAstNode {
private:
    VnlcAstNode() = delete;
    void resetPosition(const VnlcToken& firstToken, const VnlcToken& lastToken) noexcept;

protected:
    unsigned int offset;
    unsigned int length;

    unsigned int line;
    unsigned int column;

    VnlcAstNode(const VnlcToken& firstToken, const VnlcToken& lastToken); // lastToken is the first token of the next node

public:
    [[nodiscard]] std::pair<unsigned int, unsigned int> locate() const noexcept;
    [[nodiscard]] unsigned int getOffset() const noexcept;
    [[nodiscard]] unsigned int getLength() const noexcept;

    virtual ~VnlcAstNode() = default;
    friend class VnlcParser;
};

#endif // VNLC_AST_NODE_HPP
