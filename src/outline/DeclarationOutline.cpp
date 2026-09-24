#include "DeclarationOutline.hpp"

namespace vnlc {
    DeclarationOutline::DeclarationOutline(const Token& firstToken, const Token& lastToken) noexcept
        : offset(firstToken.getOffset()),
          length(lastToken.getOffset() - firstToken.getOffset()),
          line(firstToken.getLine()),
          column(firstToken.getColumn()) {}

    std::pair<std::size_t, std::size_t> DeclarationOutline::locate() const noexcept {
        return { line, column };
    }

    std::size_t DeclarationOutline::getOffset() const noexcept {
        return offset;
    }

    std::size_t DeclarationOutline::getLength() const noexcept {
        return length;
    }
} // namespace vnlc
