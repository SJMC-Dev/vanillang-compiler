#include "ImportedParameter.hpp"

namespace vnlc {
    ImportedParameter::ImportedParameter(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata)
        : ImportedIdentifier(name, std::move(metadata)),
          type(type) {}

    ImportedParameter::ImportedParameter(std::string_view name, std::string_view type) : ImportedIdentifier(name), type(type) {}

    std::string_view ImportedParameter::getType() const {
        return type;
    }
} // namespace vnlc
