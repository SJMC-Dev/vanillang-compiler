#include "ParseResult.hpp"

namespace vnlc {
    ParseResult::ParseResult(std::unique_ptr<ModuleNode>&& moduleNode, std::vector<Diagnostic>&& errors) noexcept : moduleNode(std::move(moduleNode)), errors(std::move(errors)) {}

    const ModuleNode& ParseResult::getModuleNode() const noexcept {
        return *moduleNode;
    }

    const std::vector<Diagnostic>& ParseResult::getErrors() const noexcept {
        return errors;
    }
} // namespace vnlc
