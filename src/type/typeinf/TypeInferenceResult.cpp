#include "TypeInferenceResult.hpp"
#include <utility>

namespace vnlc {
    TypeInferenceResult::TypeInferenceResult(std::optional<std::unique_ptr<Type>> type) : type(std::move(type)) {}
} // namespace vnlc
