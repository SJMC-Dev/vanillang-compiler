#ifndef VNLC_SEMANTIC_TYPE_HPP
#define VNLC_SEMANTIC_TYPE_HPP

#include <string_view>

namespace vnlc {
    class SemanticType {
    private:
        bool optional;

    protected:
        explicit SemanticType(bool optional);

    public:
        [[nodiscard]] virtual std::string_view getFullTypeName() const noexcept = 0;
        [[nodiscard]] bool isOptional() const noexcept;

        virtual ~SemanticType() = default;
    };
} // namespace vnlc

#endif // VNLC_SEMANTIC_TYPE_HPP
