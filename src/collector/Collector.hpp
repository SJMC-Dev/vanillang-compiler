#ifndef VNLC_COLLECTOR_HPP
#define VNLC_COLLECTOR_HPP

#include "collector/CollectionResult.hpp"
#include "config/Config.hpp"
#include "lexer/Lexer.hpp"
#include "outline/ClassDeclarationOutline.hpp"
#include "outline/EnumDeclarationOutline.hpp"
#include "outline/InterfaceDeclarationOutline.hpp"
#include "outline/TypeAliasDeclarationOutline.hpp"
#include "token/Token.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace vnlc {
    class Collector {
    private:
        Lexer lexer;

        std::vector<Token> tokenBuffer;
        std::size_t currentTokenIndex;
        std::size_t bufferSize;
        bool endsWithNewlineOrEOF;

        [[nodiscard]] bool hasNextToken() const;
        [[nodiscard]] const Token& peek() const;

        void fillBuffer();
        void advance();
        void advanceRaw();
        void skipNewlines();

        [[nodiscard]] bool check(TokenKind expectedKind);
        [[nodiscard]] bool match(TokenKind expectedKind);

        [[nodiscard]] std::string consumeIdentifier();

        void collectImport(const Config& config, std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports, std::vector<Diagnostic>& errors);
        void collectImportPath(std::vector<std::string> prefix, bool allowSelf, std::vector<std::vector<std::string>>& paths);
        void collectImportPathList(std::vector<std::string> prefix, std::vector<std::vector<std::string>>& paths);

        [[nodiscard]] std::unique_ptr<TypeDeclarationOutline> collectTypeDeclaration();
        [[nodiscard]] std::unique_ptr<ClassDeclarationOutline> collectClassDeclaration();
        [[nodiscard]] std::unique_ptr<InterfaceDeclarationOutline> collectInterfaceDeclaration();
        [[nodiscard]] std::unique_ptr<EnumDeclarationOutline> collectEnumDeclaration();
        [[nodiscard]] std::unique_ptr<TypeAliasDeclarationOutline> collectTypeAliasDeclaration();

        void skipBalanced(TokenKind leftKind, TokenKind rightKind);
        void skipMetadata();
        void skipUntilLeftBrace();

        [[nodiscard]] static std::string joinPath(const std::vector<std::string>& path);
        [[nodiscard]] static Diagnostic makeError(const Token& token, std::string_view message);
        static void mergeImports(std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& target, std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& source);

        static void collectTypeDependencies(std::string_view type, std::unordered_set<std::string>& dependencies);
        static void collectImportDependencies(const ImportedItem& item, std::unordered_set<std::string>& dependencies, std::unordered_set<const ImportedModule*>& visitedModules);

        [[nodiscard]] CollectionResult collectModule(const Config& config);

    public:
        explicit Collector(Lexer&& lexer, std::size_t maxBufferSize = 3);

        [[nodiscard]] CollectionResult collect(const Config& config);
    };
} // namespace vnlc

#endif // VNLC_COLLECTOR_HPP
