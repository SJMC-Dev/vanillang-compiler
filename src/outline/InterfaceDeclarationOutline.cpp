#include "InterfaceDeclarationOutline.hpp"

namespace vnlc {
    InterfaceDeclarationOutline::InterfaceDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          name(std::move(name)) {}

    std::string_view InterfaceDeclarationOutline::getName() const noexcept {
        return name;
    }
} // namespace vnlc
