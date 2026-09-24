#include "ClassDeclarationOutline.hpp"

namespace vnlc {
    ClassDeclarationOutline::ClassDeclarationOutline(bool final, std::string&& name, std::vector<std::string>&& genericParameterNames, const Token& firstToken, const Token& lastToken) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          final(final),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)) {}

    const bool ClassDeclarationOutline::isFinal() const noexcept {
        return final;
    }

    std::string_view ClassDeclarationOutline::getName() const noexcept {
        return name;
    }

    const std::vector<std::string>& ClassDeclarationOutline::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }
} // namespace vnlc
