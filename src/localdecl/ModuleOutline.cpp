#include "ModuleOutline.hpp"

namespace vnlc {
    ModuleOutline::ModuleOutline(
        std::string&& name,
        std::string&& fullName,
        std::vector<std::unique_ptr<TypeDeclarationOutline>>&& typeDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : DeclarationOutline(firstToken, lastToken),
          name(std::move(name)),
          fullName(std::move(fullName)),
          typeDeclarations(std::move(typeDeclarations)) {}

    std::string_view ModuleOutline::getName() const noexcept {
        return name;
    }

    std::string_view ModuleOutline::getFullName() const noexcept {
        return fullName;
    }

    const std::vector<std::unique_ptr<TypeDeclarationOutline>>& ModuleOutline::getTypeDeclarations() const noexcept {
        return typeDeclarations;
    }
} // namespace vnlc
