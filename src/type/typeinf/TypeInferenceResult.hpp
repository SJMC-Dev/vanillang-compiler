#ifndef VNLC_TYPE_INFERENCE_RESULT_HPP
#define VNLC_TYPE_INFERENCE_RESULT_HPP

#include "type/SemanticType.hpp"
#include <memory>
#include <optional>

namespace vnlc {
    struct TypeInferenceResult {
        std::optional<std::unique_ptr<SemanticType>> type; // std::nullopt if type inference failed

        TypeInferenceResult(std::optional<std::unique_ptr<SemanticType>> type);
        TypeInferenceResult(const TypeInferenceResult&) = delete;
        TypeInferenceResult& operator=(const TypeInferenceResult&) = delete;
        TypeInferenceResult(TypeInferenceResult&&) noexcept = default;
        TypeInferenceResult& operator=(TypeInferenceResult&&) noexcept = default;

        static TypeInferenceResult failed() {
            return TypeInferenceResult(std::nullopt);
        }
    };
} // namespace vnlc

#endif // VNLC_TYPE_INFERENCE_RESULT_HPP