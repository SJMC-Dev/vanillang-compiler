#include "TypeAliasDeclarationOutline.hpp"

namespace vnlc {
    TypeAliasDeclarationOutline::TypeAliasDeclarationOutline(
        std::string&& aliasName,
        std::vector<std::string>&& genericParameterNames,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          aliasName(std::move(aliasName)),
          genericParameterNames(std::move(genericParameterNames)) {}

    std::string_view TypeAliasDeclarationOutline::getAliasName() const noexcept {
        return aliasName;
    }

    const std::vector<std::string>& TypeAliasDeclarationOutline::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }
} // namespace vnlc
