#ifndef VNLC_TYPE_ALIAS_DECLARATION_OUTLINE_HPP
#define VNLC_TYPE_ALIAS_DECLARATION_OUTLINE_HPP

#include "outline/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class TypeAliasDeclarationOutline : public TypeDeclarationOutline {
    private:
        TypeAliasDeclarationOutline() = delete;

        std::string aliasName;
        std::vector<std::string> genericParameterNames;

    public:
        TypeAliasDeclarationOutline(std::string&& aliasName, std::vector<std::string>&& genericParameterNames, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] std::string_view getAliasName() const noexcept;
        [[nodiscard]] const std::vector<std::string>& getGenericParameterNames() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_ALIAS_DECLARATION_OUTLINE_HPP
