#include "ImportedLet.hpp"

namespace vnlc {
    ImportedLet::ImportedLet(std::string_view name, std::string_view type, std::unordered_map<std::string, std::optional<std::string>>&& metadata)
        : ImportedIdentifier(name, std::move(metadata)),
          type(type) {}

    ImportedLet::ImportedLet(std::string_view name, std::string_view type) : ImportedIdentifier(name), type(type) {}

    std::string_view ImportedLet::getType() const {
        return type;
    }
} // namespace vnlc
