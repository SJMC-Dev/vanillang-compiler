#ifndef VNLC_CLASS_DECLARATION_OUTLINE_HPP
#define VNLC_CLASS_DECLARATION_OUTLINE_HPP

#include "outline/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class ClassDeclarationOutline : public TypeDeclarationOutline {
    private:
        ClassDeclarationOutline() = delete;

        std::string name;

    public:
        ClassDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CLASS_DECLARATION_OUTLINE_HPP
