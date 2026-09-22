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
        std::size_t offset;
        std::size_t length;

        std::size_t line;
        std::size_t column;

        AstNode(const Token& firstToken, const Token& lastToken); // lastToken is the first token of the next node

    public:
        [[nodiscard]] std::pair<std::size_t, std::size_t> locate() const noexcept;
        [[nodiscard]] std::size_t getOffset() const noexcept;
        [[nodiscard]] std::size_t getLength() const noexcept;

        virtual ~AstNode() = default;
        friend class Parser;
    };
} // namespace vnlc

#endif // VNLC_AST_NODE_HPP
