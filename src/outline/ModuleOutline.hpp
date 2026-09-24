#ifndef VNLC_MODULE_OUTLINE_HPP
#define VNLC_MODULE_OUTLINE_HPP

#include "outline/DeclarationOutline.hpp"
#include "outline/TypeDeclarationOutline.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class ModuleOutline : public DeclarationOutline {
    private:
        ModuleOutline() = delete;

        std::string name;
        std::string fullName;
        std::vector<std::unique_ptr<TypeDeclarationOutline>> typeDeclarations;

    public:
        ModuleOutline(
            std::string&& name,
            std::string&& fullName,
            std::vector<std::unique_ptr<TypeDeclarationOutline>>&& typeDeclarations,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] std::string_view getFullName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<TypeDeclarationOutline>>& getTypeDeclarations() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_MODULE_OUTLINE_HPP
