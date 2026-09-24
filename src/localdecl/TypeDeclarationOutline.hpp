#ifndef VNLC_TYPE_DECLARATION_OUTLINE_HPP
#define VNLC_TYPE_DECLARATION_OUTLINE_HPP

#include "localdecl/DeclarationOutline.hpp"

namespace vnlc {
    class TypeDeclarationOutline : public DeclarationOutline {
    private:
        TypeDeclarationOutline() = delete;

    protected:
        TypeDeclarationOutline(const Token& firstToken, const Token& lastToken) noexcept;
    };
} // namespace vnlc

#endif // VNLC_TYPE_DECLARATION_OUTLINE_HPP
