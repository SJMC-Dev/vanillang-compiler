#include "TypeAliasDeclarationOutline.hpp"

namespace vnlc {
    TypeAliasDeclarationOutline::TypeAliasDeclarationOutline(std::string&& aliasName, const Token& firstToken, const Token& lastToken) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          aliasName(std::move(aliasName)) {}

    std::string_view TypeAliasDeclarationOutline::getAliasName() const noexcept {
        return aliasName;
    }
} // namespace vnlc
