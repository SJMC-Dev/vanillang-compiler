#ifndef VNLC_VOID_TYPE_HPP
#define VNLC_VOID_TYPE_HPP

#include "type/Type.hpp"

namespace vnlc {
    class VoidType : public Type {
    public:
        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
    };
} // namespace vnlc

#endif // VNLC_VOID_TYPE_HPP
