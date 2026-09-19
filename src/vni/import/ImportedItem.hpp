#ifndef VNLC_IMPORTED_ITEM_HPP
#define VNLC_IMPORTED_ITEM_HPP

#include <string>
#include <string_view>

namespace vnlc {
    class ImportedItem {
    private:
        std::string name;

    protected:
        ImportedItem(std::string_view name);

    public:
        [[nodiscard]] std::string_view getName() const;

        virtual ~ImportedItem() = default;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_ITEM_HPP