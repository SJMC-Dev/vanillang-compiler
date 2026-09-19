#ifndef VNLC_PARSER_HPP
#define VNLC_PARSER_HPP

#include "ast/module/ModuleNode.hpp"
#include "config/Config.hpp"
#include "lexer/Lexer.hpp"
#include "parser/inherited/ParsingContexts.hpp"
#include "parser/synthesized/ParsingResults.hpp"
#include "token/Token.hpp"
#include <memory>
#include <unordered_set>
#include <vector>

namespace vnlc {
    class Parser {
    private:
        Lexer lexer;

        std::vector<Token> tokenBuffer;
        unsigned int currentTokenIndex;
        unsigned int bufferSize;
        bool endsWithNewlineOrEOF;

        [[nodiscard]] bool hasNextToken() const;
        [[nodiscard]] const Token& peek() const;
        [[nodiscard]] const Token& peek(unsigned int offset) const;

        void fillBuffer();
        void advance();
        void advanceRaw();
        void skipNewlines();

        [[nodiscard]] bool check(TokenType expectedType);
        [[nodiscard]] bool checkGeneralizedIdentifier();
        [[nodiscard]] bool checkAny(const std::unordered_set<TokenType>& expectedTypes);
        [[nodiscard]] bool match(TokenType expectedType);
        [[nodiscard]] bool matchAny(const std::unordered_set<TokenType>& expectedTypes);
        [[nodiscard]] bool consumeRightAngleInType();

        [[nodiscard]] std::unique_ptr<IdentifierNode> constructCurrentIdentifierNode();

        [[nodiscard]] std::string generateNamespaceIdFromTypeName(const TypeNode& typeNode);

        [[nodiscard]] ModuleParsingResult parseModule(ModuleParsingContext context);
        [[nodiscard]] TopIdentifierDeclarationParsingResult parseTopIdentifierDeclaration();
        [[nodiscard]] ImportDeclarationParsingResult parseImportDeclaration();
        [[nodiscard]] ExportDeclarationParsingResult parseExportDeclaration();
        [[nodiscard]] VariableDeclarationParsingResult parseVariableDeclaration(VariableDeclarationParsingContext context);
        [[nodiscard]] FunctionDeclarationParsingResult parseFunctionDeclaration(FunctionDeclarationParsingContext context);
        [[nodiscard]] TypeDeclarationParsingResult parseTypeDeclaration(TypeDeclarationParsingContext context);
        [[nodiscard]] PropertyDeclarationParsingResult parsePropertyDeclaration(PropertyDeclarationParsingContext context);
        [[nodiscard]] InterfaceMethodDeclarationParsingResult parseInterfaceMethodDeclaration();
        [[nodiscard]] MetadataParsingResult parseMetadata();
        [[nodiscard]] VariableDeclarationPrimaryParsingResult parseVariableDeclarationPrimary(VariableDeclarationPrimaryParsingContext context);
        [[nodiscard]] RegularFunctionDeclarationParsingResult parseRegularFunctionDeclaration(RegularFunctionDeclarationParsingContext context);
        [[nodiscard]] NativeFunctionDeclarationParsingResult parseNativeFunctionDeclaration(NativeFunctionDeclarationParsingContext context);
        [[nodiscard]] ParameterListParsingResult parseParameterList();
        [[nodiscard]] ClassDeclarationParsingResult parseClassDeclaration(ClassDeclarationParsingContext context);
        [[nodiscard]] InterfaceDeclarationParsingResult parseInterfaceDeclaration(InterfaceDeclarationParsingContext context);
        [[nodiscard]] EnumDeclarationParsingResult parseEnumDeclaration(EnumDeclarationParsingContext context);
        [[nodiscard]] TypeAliasDeclarationParsingResult parseTypeAliasDeclaration(TypeAliasDeclarationParsingContext context);
        [[nodiscard]] ImportPathParsingResult parseImportPath();
        [[nodiscard]] ExportListParsingResult parseExportList();
        [[nodiscard]] MetadataTermParsingResult parseMetadataTerm();
        [[nodiscard]] FunctionSignatureParsingResult parseFunctionSignature();
        [[nodiscard]] TypeParsingResult parseType();
        [[nodiscard]] ParameterParsingResult parseParameter();
        [[nodiscard]] GenericParameterListParsingResult parseGenericParameterList();
        [[nodiscard]] GenericArgumentListParsingResult parseGenericArgumentList();
        [[nodiscard]] ImportPathListParsingResult parseImportPathList();
        [[nodiscard]] FunctionBodyParsingResult parseFunctionBody();
        [[nodiscard]] ClassBodyParsingResult parseClassBody();
        [[nodiscard]] InterfaceBodyParsingResult parseInterfaceBody();
        [[nodiscard]] EnumBodyParsingResult parseEnumBody();
        [[nodiscard]] ImportPathItemParsingResult parseImportPathItem();
        [[nodiscard]] ClassMemberParsingResult parseClassMember();
        [[nodiscard]] ConstructorParsingResult parseConstructor(ConstructorParsingContext context);
        [[nodiscard]] EnumMemberDeclarationParsingResult parseEnumMemberDeclaration();
        [[nodiscard]] EnumAssociatedValueListParsingResult parseEnumAssociatedValueList();
        [[nodiscard]] EnumAssociatedValueParsingResult parseEnumAssociatedValue();
        [[nodiscard]] ExpressionParsingResult parseExpression();
        [[nodiscard]] AssignmentExpressionParsingResult parseAssignmentExpression();
        [[nodiscard]] ConditionalExpressionParsingResult parseConditionalExpression();
        [[nodiscard]] NullishCoalescingExpressionParsingResult parseNullishCoalescingExpression();
        [[nodiscard]] LogicalOrExpressionParsingResult parseLogicalOrExpression();
        [[nodiscard]] LogicalAndExpressionParsingResult parseLogicalAndExpression();
        [[nodiscard]] EqualityExpressionParsingResult parseEqualityExpression();
        [[nodiscard]] RelationalExpressionParsingResult parseRelationalExpression();
        [[nodiscard]] RangeExpressionParsingResult parseRangeExpression();
        [[nodiscard]] BitwiseOrExpressionParsingResult parseBitwiseOrExpression();
        [[nodiscard]] BitwiseXorExpressionParsingResult parseBitwiseXorExpression();
        [[nodiscard]] BitwiseAndExpressionParsingResult parseBitwiseAndExpression();
        [[nodiscard]] ShiftExpressionParsingResult parseShiftExpression();
        [[nodiscard]] AdditiveExpressionParsingResult parseAdditiveExpression();
        [[nodiscard]] MultiplicativeExpressionParsingResult parseMultiplicativeExpression();
        [[nodiscard]] UnaryExpressionParsingResult parseUnaryExpression();
        [[nodiscard]] ExponentialExpressionParsingResult parseExponentialExpression();
        [[nodiscard]] PostfixExpressionParsingResult parsePostfixExpression();
        [[nodiscard]] PrimaryExpressionParsingResult parsePrimaryExpression();
        [[nodiscard]] LiteralParsingResult parseLiteral();
        [[nodiscard]] StringParsingResult parseString();
        [[nodiscard]] BooleanParsingResult parseBoolean();
        [[nodiscard]] ListLikeLiteralParsingResult parseListLikeLiteral();
        [[nodiscard]] DictLiteralParsingResult parseDictLiteral();
        [[nodiscard]] SelectorParsingResult parseSelector();
        [[nodiscard]] ArgumentListParsingResult parseArgumentList();
        [[nodiscard]] InterpolationParsingResult parseInterpolation();
        [[nodiscard]] DictEntryParsingResult parseDictEntry();
        [[nodiscard]] SelectorArgumentListParsingResult parseSelectorArgumentList();
        [[nodiscard]] SelectorArgumentParsingResult parseSelectorArgument();
        [[nodiscard]] StatementParsingResult parseStatement();
        [[nodiscard]] ExpressionStatementParsingResult parseExpressionStatement();
        [[nodiscard]] VariableDeclarationStatementParsingResult parseVariableDeclarationStatement();
        [[nodiscard]] BlockStatementParsingResult parseBlockStatement();
        [[nodiscard]] ControlFlowStatementParsingResult parseControlFlowStatement();
        [[nodiscard]] IfStatementParsingResult parseIfStatement();
        [[nodiscard]] SwitchStatementParsingResult parseSwitchStatement();
        [[nodiscard]] WhileStatementParsingResult parseWhileStatement(WhileStatementParsingContext context);
        [[nodiscard]] ForStatementParsingResult parseForStatement(ForStatementParsingContext context);
        [[nodiscard]] ReturnStatementParsingResult parseReturnStatement();
        [[nodiscard]] BreakStatementParsingResult parseBreakStatement();
        [[nodiscard]] ContinueStatementParsingResult parseContinueStatement();
        [[nodiscard]] ReloadStatementParsingResult parseReloadStatement();
        [[nodiscard]] SwitchCaseParsingResult parseSwitchCase();

    public:
        explicit Parser(Lexer&& lexer, unsigned int maxBufferSize = 3);

        [[nodiscard]] std::unique_ptr<ModuleNode> parse(const Config& config);
    };
} // namespace vnlc

#endif // VNLC_PARSER_HPP
