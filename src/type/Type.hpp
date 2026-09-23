#ifndef VNLC_TYPE_HPP
#define VNLC_TYPE_HPP

#include <string_view>

namespace vnlc {
    class Type {
    public:
        [[nodiscard]] virtual std::string_view getFullTypeName() const noexcept = 0;

        virtual ~Type() = default;
    };
} // namespace vnlc

#endif // VNLC_TYPE_HPP
