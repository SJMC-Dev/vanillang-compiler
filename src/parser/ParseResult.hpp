#ifndef VNLC_PARSE_RESULT_HPP
#define VNLC_PARSE_RESULT_HPP

#include "ast/module/ModuleNode.hpp"
#include "diagnostic/Diagnostic.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class ParseResult {
    private:
        std::unique_ptr<ModuleNode> moduleNode;
        std::vector<Diagnostic> errors;

    public:
        ParseResult(std::unique_ptr<ModuleNode>&& moduleNode, std::vector<Diagnostic>&& errors) noexcept;

        ParseResult(const ParseResult&) = delete;
        ParseResult& operator=(const ParseResult&) = delete;
        ParseResult(ParseResult&&) noexcept = default;
        ParseResult& operator=(ParseResult&&) noexcept = default;

        [[nodiscard]] const ModuleNode& getModuleNode() const noexcept;
        [[nodiscard]] const std::vector<Diagnostic>& getErrors() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_PARSE_RESULT_HPP
