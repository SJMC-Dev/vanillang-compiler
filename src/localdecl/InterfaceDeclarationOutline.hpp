#ifndef VNLC_INTERFACE_DECLARATION_OUTLINE_HPP
#define VNLC_INTERFACE_DECLARATION_OUTLINE_HPP

#include "localdecl/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class InterfaceDeclarationOutline : public TypeDeclarationOutline {
    private:
        InterfaceDeclarationOutline() = delete;

        std::string name;
        std::vector<std::string> genericParameterNames;

    public:
        InterfaceDeclarationOutline(std::string&& name, std::vector<std::string>&& genericParameterNames, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] const std::vector<std::string>& getGenericParameterNames() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_DECLARATION_OUTLINE_HPP
