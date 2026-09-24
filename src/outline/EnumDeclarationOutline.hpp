#ifndef VNLC_ENUM_DECLARATION_OUTLINE_HPP
#define VNLC_ENUM_DECLARATION_OUTLINE_HPP

#include "outline/EnumMemberDeclarationOutline.hpp"
#include "outline/TypeDeclarationOutline.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class EnumDeclarationOutline : public TypeDeclarationOutline {
    private:
        EnumDeclarationOutline() = delete;

        std::string name;
        std::vector<std::unique_ptr<EnumMemberDeclarationOutline>> memberDeclarations;

    public:
        EnumDeclarationOutline(std::string&& name, std::vector<std::unique_ptr<EnumMemberDeclarationOutline>>&& memberDeclarations, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<EnumMemberDeclarationOutline>>& getMemberDeclarations() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_ENUM_DECLARATION_OUTLINE_HPP
