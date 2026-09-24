#include "EnumDeclarationOutline.hpp"

namespace vnlc {
    EnumDeclarationOutline::EnumDeclarationOutline(
        std::string&& name,
        std::vector<std::unique_ptr<EnumMemberDeclarationOutline>>&& memberDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          name(std::move(name)),
          memberDeclarations(std::move(memberDeclarations)) {
        for (auto& memberDeclaration : this->memberDeclarations) {
            memberDeclaration->enumDeclaration = this;
        }
    }

    std::string_view EnumDeclarationOutline::getName() const noexcept {
        return name;
    }

    const std::vector<std::unique_ptr<EnumMemberDeclarationOutline>>& EnumDeclarationOutline::getMemberDeclarations() const noexcept {
        return memberDeclarations;
    }
} // namespace vnlc
