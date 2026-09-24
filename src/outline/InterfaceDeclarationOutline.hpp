#ifndef VNLC_INTERFACE_DECLARATION_OUTLINE_HPP
#define VNLC_INTERFACE_DECLARATION_OUTLINE_HPP

#include "outline/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class InterfaceDeclarationOutline : public TypeDeclarationOutline {
    private:
        InterfaceDeclarationOutline() = delete;

        std::string name;

    public:
        InterfaceDeclarationOutline(std::string&& name, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_DECLARATION_OUTLINE_HPP
