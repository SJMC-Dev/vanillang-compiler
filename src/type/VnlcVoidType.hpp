#ifndef VNLC_VOID_TYPE_HPP
#define VNLC_VOID_TYPE_HPP

#include "type/VnlcSemanticType.hpp"

class VnlcVoidType : public VnlcSemanticType {
public:
    VnlcVoidType();

    [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
};

#endif // VNLC_VOID_TYPE_HPP