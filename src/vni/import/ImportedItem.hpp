#ifndef VNLC_IMPORTED_ITEM_HPP
#define VNLC_IMPORTED_ITEM_HPP

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class ImportedPackage;

    class ImportedItem {
    private:
        std::string name;

    protected:
        ImportedItem(std::string_view name);

    public:
        [[nodiscard]] std::string_view getName() const;
        [[nodiscard]] const ImportedItem* getChildByName(std::string_view name) const;
        [[nodiscard]] static const ImportedItem*
        getImportedItemByFullPath(const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports, const std::vector<std::string>& fullPath);

        virtual ~ImportedItem() = default;
    };
} // namespace vnlc

#endif // VNLC_IMPORTED_ITEM_HPP
