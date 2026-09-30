#ifndef VNLC_FUNCTION_SYMBOL_HPP
#define VNLC_FUNCTION_SYMBOL_HPP

#include "symbol/RegularSymbol.hpp"
#include "symbol/Symbol.hpp"
#include <string>
#include <string_view>
#include <unordered_map>

namespace vnlc {
    class FunctionSymbol : public Symbol {
    private:
        std::string name;
        std::unordered_map<std::string, RegularSymbol> overloadings;

    public:
        explicit FunctionSymbol(std::string_view name);

        bool addOverloading(RegularSymbol&& overloading);
        [[nodiscard]] const RegularSymbol* getOverloadingByInternalName(std::string_view internalName) const;
        [[nodiscard]] const std::unordered_map<std::string, RegularSymbol>& getOverloadings() const noexcept;
        [[nodiscard]] bool isUnique() const noexcept;
        [[nodiscard]] std::string_view getName() const noexcept override;
    };
} // namespace vnlc

#endif // VNLC_FUNCTION_SYMBOL_HPP
