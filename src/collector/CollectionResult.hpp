#ifndef VNLC_COLLECTION_RESULT_HPP
#define VNLC_COLLECTION_RESULT_HPP

#include "diagnostic/Diagnostic.hpp"
#include "outline/ModuleOutline.hpp"
#include "vni/import/ImportedPackage.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace vnlc {
    class CollectionResult {
    private:
        std::unique_ptr<ModuleOutline> moduleOutline;
        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        std::vector<Diagnostic> errors;

    public:
        CollectionResult(
            std::unique_ptr<ModuleOutline>&& moduleOutline,
            std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& imports,
            std::vector<Diagnostic>&& errors
        ) noexcept;

        CollectionResult(const CollectionResult&) = delete;
        CollectionResult& operator=(const CollectionResult&) = delete;
        CollectionResult(CollectionResult&&) noexcept = default;
        CollectionResult& operator=(CollectionResult&&) noexcept = default;

        [[nodiscard]] ModuleOutline& getModuleOutline() noexcept;
        [[nodiscard]] const ModuleOutline& getModuleOutline() const noexcept;
        [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& getImports() const noexcept;
        [[nodiscard]] const std::vector<Diagnostic>& getErrors() const noexcept;

        [[nodiscard]] std::unique_ptr<ModuleOutline> takeModuleOutline() noexcept;
        [[nodiscard]] std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> takeImports() noexcept;
        [[nodiscard]] std::vector<Diagnostic> takeErrors() noexcept;
    };
} // namespace vnlc

#endif // VNLC_COLLECTION_RESULT_HPP
