#include "ClassDeclarationOutline.hpp"

namespace vnlc {
    ClassDeclarationOutline::ClassDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          name(std::move(name)) {}

    std::string_view ClassDeclarationOutline::getName() const noexcept {
        return name;
    }
} // namespace vnlc
