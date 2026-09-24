#include "EnumDeclarationOutline.hpp"

namespace vnlc {
    EnumDeclarationOutline::EnumDeclarationOutline(
        std::string&& name,
        std::vector<std::string>&& genericParameterNames,
        std::vector<std::unique_ptr<EnumMemberDeclarationOutline>>&& memberDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)),
          memberDeclarations(std::move(memberDeclarations)) {
        for (auto& memberDeclaration : this->memberDeclarations) {
            memberDeclaration->enumDeclaration = this;
        }
    }

    std::string_view EnumDeclarationOutline::getName() const noexcept {
        return name;
    }

    const std::vector<std::string>& EnumDeclarationOutline::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }

    const std::vector<std::unique_ptr<EnumMemberDeclarationOutline>>& EnumDeclarationOutline::getMemberDeclarations() const noexcept {
        return memberDeclarations;
    }
} // namespace vnlc
