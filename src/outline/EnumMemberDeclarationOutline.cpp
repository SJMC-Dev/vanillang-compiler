#include "EnumMemberDeclarationOutline.hpp"

namespace vnlc {
    EnumMemberDeclarationOutline::EnumMemberDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          name(std::move(name)),
          enumDeclaration(nullptr) {}

    std::string_view EnumMemberDeclarationOutline::getName() const noexcept {
        return name;
    }

    const EnumDeclarationOutline* EnumMemberDeclarationOutline::getEnumDeclaration() const noexcept {
        return enumDeclaration;
    }
} // namespace vnlc
