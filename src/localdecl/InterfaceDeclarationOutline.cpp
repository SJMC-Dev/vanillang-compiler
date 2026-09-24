#include "InterfaceDeclarationOutline.hpp"

namespace vnlc {
    InterfaceDeclarationOutline::InterfaceDeclarationOutline(std::string&& name, std::vector<std::string>&& genericParameterNames, const Token& firstToken, const Token& lastToken) noexcept
        : TypeDeclarationOutline(firstToken, lastToken),
          name(std::move(name)),
          genericParameterNames(std::move(genericParameterNames)) {}

    std::string_view InterfaceDeclarationOutline::getName() const noexcept {
        return name;
    }

    const std::vector<std::string>& InterfaceDeclarationOutline::getGenericParameterNames() const noexcept {
        return genericParameterNames;
    }
} // namespace vnlc
