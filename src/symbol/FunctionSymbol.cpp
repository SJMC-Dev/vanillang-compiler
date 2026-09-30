#include "FunctionSymbol.hpp"

namespace vnlc {
    FunctionSymbol::FunctionSymbol(std::string_view name) : name(name) {}

    bool FunctionSymbol::addOverloading(RegularSymbol&& overloading) {
        const std::string internalName(overloading.getName());
        return overloadings.emplace(internalName, std::move(overloading)).second;
    }

    const RegularSymbol* FunctionSymbol::getOverloadingByInternalName(std::string_view internalName) const {
        auto it = overloadings.find(std::string(internalName));
        if (it != overloadings.end()) {
            return &it->second;
        }
        return nullptr;
    }

    const std::unordered_map<std::string, RegularSymbol>& FunctionSymbol::getOverloadings() const noexcept {
        return overloadings;
    }

    bool FunctionSymbol::isUnique() const noexcept {
        return overloadings.size() == 1;
    }

    std::string_view FunctionSymbol::getName() const noexcept {
        return name;
    }
} // namespace vnlc
