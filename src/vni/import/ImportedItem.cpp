#include "ImportedItem.hpp"

namespace vnlc {
    ImportedItem::ImportedItem(std::string_view name) : name(name) {}

    std::string_view ImportedItem::getName() const {
        return name;
    }
} // namespace vnlc
