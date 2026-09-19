#ifndef VNLC_VOID_TYPE_HPP
#define VNLC_VOID_TYPE_HPP

#include "type/SemanticType.hpp"

namespace vnlc {
    class VoidType : public SemanticType {
    public:
        VoidType();

        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
    };
} // namespace vnlc

#endif // VNLC_VOID_TYPE_HPP