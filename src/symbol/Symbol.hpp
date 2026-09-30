#ifndef VNLC_SYMBOL_HPP
#define VNLC_SYMBOL_HPP

#include <string_view>

namespace vnlc {
    class Symbol {
    public:
        Symbol() = default;
        virtual ~Symbol() = default;

        Symbol(const Symbol&) = default;
        Symbol(Symbol&&) noexcept = default;
        Symbol& operator=(const Symbol&) = default;
        Symbol& operator=(Symbol&&) noexcept = default;

        [[nodiscard]] virtual std::string_view getName() const noexcept = 0;
    };
} // namespace vnlc

#endif // VNLC_SYMBOL_HPP
