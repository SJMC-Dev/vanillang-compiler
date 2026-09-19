#ifndef VNLC_SEMANTIC_TYPE_HPP
#define VNLC_SEMANTIC_TYPE_HPP

#include <string_view>

namespace vnlc {
    class SemanticType {
    protected:
        SemanticType();

    public:
        [[nodiscard]] virtual std::string_view getFullTypeName() const noexcept = 0;

        virtual ~SemanticType() = default;
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_TYPE_HPP