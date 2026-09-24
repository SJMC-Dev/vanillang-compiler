#ifndef VNLC_CLASS_DECLARATION_OUTLINE_HPP
#define VNLC_CLASS_DECLARATION_OUTLINE_HPP

#include "localdecl/TypeDeclarationOutline.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class ClassDeclarationOutline : public TypeDeclarationOutline {
    private:
        ClassDeclarationOutline() = delete;

        bool final;
        std::string name;
        std::vector<std::string> genericParameterNames;

    public:
        ClassDeclarationOutline(bool final, std::string&& name, std::vector<std::string>&& genericParameterNames, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const bool isFinal() const noexcept;
        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] const std::vector<std::string>& getGenericParameterNames() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CLASS_DECLARATION_OUTLINE_HPP
