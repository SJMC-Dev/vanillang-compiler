#ifndef VNLC_AST_NODE_HPP
#define VNLC_AST_NODE_HPP

#include "token/Token.hpp"
#include <utility>

namespace vnlc {
    class AstNode {
    private:
        AstNode() = delete;
        void resetPosition(const Token& firstToken, const Token& lastToken) noexcept;

    protected:
        unsigned int offset;
        unsigned int length;

        unsigned int line;
        unsigned int column;

        AstNode(const Token& firstToken, const Token& lastToken); // lastToken is the first token of the next node

    public:
        [[nodiscard]] std::pair<unsigned int, unsigned int> locate() const noexcept;
        [[nodiscard]] unsigned int getOffset() const noexcept;
        [[nodiscard]] unsigned int getLength() const noexcept;

        virtual ~AstNode() = default;
        friend class Parser;
    };
} // namespace vnlc

#endif // VNLC_AST_NODE_HPP
