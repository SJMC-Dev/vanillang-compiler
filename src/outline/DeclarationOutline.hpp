#ifndef VNLC_DECLARATION_OUTLINE_HPP
#define VNLC_DECLARATION_OUTLINE_HPP

#include "token/Token.hpp"
#include <cstddef>
#include <utility>

namespace vnlc {
    class DeclarationOutline {
    private:
        DeclarationOutline() = delete;

        std::size_t offset;
        std::size_t length;

        std::size_t line;
        std::size_t column;

    protected:
        DeclarationOutline(const Token& firstToken, const Token& lastToken) noexcept;

    public:
        [[nodiscard]] std::pair<std::size_t, std::size_t> locate() const noexcept;
        [[nodiscard]] std::size_t getOffset() const noexcept;
        [[nodiscard]] std::size_t getLength() const noexcept;

        virtual ~DeclarationOutline() = default;
    };
} // namespace vnlc

#endif // VNLC_DECLARATION_OUTLINE_HPP
