#include "TypeInferenceResult.hpp"
#include <utility>

namespace vnlc {
    TypeInferenceResult::TypeInferenceResult(std::optional<std::unique_ptr<SemanticType>> type) : type(std::move(type)) {}
} // namespace vnlc
