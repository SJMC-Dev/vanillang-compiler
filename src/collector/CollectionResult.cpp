#include "CollectionResult.hpp"

namespace vnlc {
    CollectionResult::CollectionResult(
        std::unique_ptr<ModuleOutline>&& moduleOutline,
        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& imports,
        std::vector<Diagnostic>&& errors
    ) noexcept
        : moduleOutline(std::move(moduleOutline)),
          imports(std::move(imports)),
          errors(std::move(errors)) {}

    ModuleOutline& CollectionResult::getModuleOutline() noexcept {
        return *moduleOutline;
    }

    const ModuleOutline& CollectionResult::getModuleOutline() const noexcept {
        return *moduleOutline;
    }

    const std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& CollectionResult::getImports() const noexcept {
        return imports;
    }

    const std::vector<Diagnostic>& CollectionResult::getErrors() const noexcept {
        return errors;
    }

    std::unique_ptr<ModuleOutline> CollectionResult::takeModuleOutline() noexcept {
        return std::move(moduleOutline);
    }

    std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> CollectionResult::takeImports() noexcept {
        return std::move(imports);
    }

    std::vector<Diagnostic> CollectionResult::takeErrors() noexcept {
        return std::move(errors);
    }
} // namespace vnlc
