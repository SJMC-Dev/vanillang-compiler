#ifndef VNLC_TYPE_INFERENCE_RESULT_HPP
#define VNLC_TYPE_INFERENCE_RESULT_HPP

#include "type/VnlcSemanticType.hpp"
#include <memory>
#include <optional>

struct VnlcTypeInferenceResult {
    std::optional<std::unique_ptr<VnlcSemanticType>> type; // std::nullopt if type inference failed

    VnlcTypeInferenceResult(std::optional<std::unique_ptr<VnlcSemanticType>> type);
    VnlcTypeInferenceResult(const VnlcTypeInferenceResult&) = delete;
    VnlcTypeInferenceResult& operator=(const VnlcTypeInferenceResult&) = delete;
    VnlcTypeInferenceResult(VnlcTypeInferenceResult&&) noexcept = default;
    VnlcTypeInferenceResult& operator=(VnlcTypeInferenceResult&&) noexcept = default;

    static VnlcTypeInferenceResult failed() {
        return VnlcTypeInferenceResult(std::nullopt);
    }
};

#endif // VNLC_TYPE_INFERENCE_RESULT_HPP