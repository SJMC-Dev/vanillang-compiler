#include "ImportedTypeAlias.hpp"

namespace vnlc {
    ImportedTypeAlias::ImportedTypeAlias(
        std::string_view name,
        std::vector<std::string>&& genericParameters,
        std::string_view originalType,
        std::unordered_map<std::string, std::optional<std::string>>&& metadata
    )
        : ImportedIdentifier(name, std::move(metadata)),
          genericParameters(std::move(genericParameters)),
          originalType(originalType) {}

    ImportedTypeAlias::ImportedTypeAlias(std::string_view name, std::vector<std::string>&& genericParameters, std::string_view originalType)
        : ImportedIdentifier(name),
          genericParameters(std::move(genericParameters)),
          originalType(originalType) {}

    const std::vector<std::string>& ImportedTypeAlias::getGenericParameters() const {
        return genericParameters;
    }

    std::string_view ImportedTypeAlias::getOriginalType() const {
        return originalType;
    }
} // namespace vnlc
