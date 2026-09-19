#include "ImportedAlias.hpp"

namespace vnlc {
    ImportedAlias::ImportedAlias(std::string_view name, std::string_view source) : ImportedIdentifier(name), source(source) {}

    std::string_view ImportedAlias::getSource() const {
        return source;
    }
} // namespace vnlc
