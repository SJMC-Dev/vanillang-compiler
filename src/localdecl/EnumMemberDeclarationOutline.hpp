#ifndef VNLC_ENUM_MEMBER_DECLARATION_OUTLINE_HPP
#define VNLC_ENUM_MEMBER_DECLARATION_OUTLINE_HPP

#include "localdecl/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class EnumDeclarationOutline;

    class EnumMemberDeclarationOutline : public TypeDeclarationOutline {
    private:
        EnumMemberDeclarationOutline() = delete;

        std::string name;
        const EnumDeclarationOutline* enumDeclaration;

        friend class EnumDeclarationOutline;

    public:
        EnumMemberDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] const EnumDeclarationOutline* getEnumDeclaration() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_ENUM_MEMBER_DECLARATION_OUTLINE_HPP
