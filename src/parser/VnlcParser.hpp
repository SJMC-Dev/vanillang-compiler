#ifndef VNLC_PARSER_HPP
#define VNLC_PARSER_HPP

#include "ast/module/VnlcModuleNode.hpp"
#include "config/VnlcConfig.hpp"
#include "lexer/VnlcLexer.hpp"
#include "parser/inherited/VnlcParsingContexts.hpp"
#include "parser/synthesized/VnlcParsingResults.hpp"
#include "token/VnlcToken.hpp"
#include <memory>
#include <unordered_set>
#include <vector>

class VnlcParser {
private:
    VnlcLexer lexer;

    std::vector<VnlcToken> tokenBuffer;
    unsigned int currentTokenIndex;
    unsigned int bufferSize;
    bool endsWithNewlineOrEOF;

    [[nodiscard]] bool hasNextToken() const;
    [[nodiscard]] const VnlcToken& peek() const;
    [[nodiscard]] const VnlcToken& peek(unsigned int offset) const;

    void fillBuffer();
    void advance();
    void advanceRaw();
    void skipNewlines();

    [[nodiscard]] bool check(VnlcTokenType expectedType);
    [[nodiscard]] bool checkGeneralizedIdentifier();
    [[nodiscard]] bool checkAny(const std::unordered_set<VnlcTokenType>& expectedTypes);
    [[nodiscard]] bool match(VnlcTokenType expectedType);
    [[nodiscard]] bool matchAny(const std::unordered_set<VnlcTokenType>& expectedTypes);
    [[nodiscard]] bool consumeRightAngleInType();

    [[nodiscard]] std::unique_ptr<VnlcIdentifierNode> constructCurrentIdentifierNode();

    [[nodiscard]] std::string generateNamespaceIdFromTypeName(const VnlcTypeNode& typeNode);

    [[nodiscard]] VnlcModuleParsingResult parseModule(VnlcModuleParsingContext context);
    [[nodiscard]] VnlcTopIdentifierDeclarationParsingResult parseTopIdentifierDeclaration();
    [[nodiscard]] VnlcImportDeclarationParsingResult parseImportDeclaration();
    [[nodiscard]] VnlcExportDeclarationParsingResult parseExportDeclaration();
    [[nodiscard]] VnlcVariableDeclarationParsingResult parseVariableDeclaration(VnlcVariableDeclarationParsingContext context);
    [[nodiscard]] VnlcFunctionDeclarationParsingResult parseFunctionDeclaration(VnlcFunctionDeclarationParsingContext context);
    [[nodiscard]] VnlcTypeDeclarationParsingResult parseTypeDeclaration(VnlcTypeDeclarationParsingContext context);
    [[nodiscard]] VnlcPropertyDeclarationParsingResult parsePropertyDeclaration(VnlcPropertyDeclarationParsingContext context);
    [[nodiscard]] VnlcInterfaceMethodDeclarationParsingResult parseInterfaceMethodDeclaration();
    [[nodiscard]] VnlcMetadataParsingResult parseMetadata();
    [[nodiscard]] VnlcVariableDeclarationPrimaryParsingResult parseVariableDeclarationPrimary(VnlcVariableDeclarationPrimaryParsingContext context);
    [[nodiscard]] VnlcRegularFunctionDeclarationParsingResult parseRegularFunctionDeclaration(VnlcRegularFunctionDeclarationParsingContext context);
    [[nodiscard]] VnlcNativeFunctionDeclarationParsingResult parseNativeFunctionDeclaration(VnlcNativeFunctionDeclarationParsingContext context);
    [[nodiscard]] VnlcParameterListParsingResult parseParameterList();
    [[nodiscard]] VnlcClassDeclarationParsingResult parseClassDeclaration(VnlcClassDeclarationParsingContext context);
    [[nodiscard]] VnlcInterfaceDeclarationParsingResult parseInterfaceDeclaration(VnlcInterfaceDeclarationParsingContext context);
    [[nodiscard]] VnlcEnumDeclarationParsingResult parseEnumDeclaration(VnlcEnumDeclarationParsingContext context);
    [[nodiscard]] VnlcTypeAliasDeclarationParsingResult parseTypeAliasDeclaration(VnlcTypeAliasDeclarationParsingContext context);
    [[nodiscard]] VnlcImportPathParsingResult parseImportPath();
    [[nodiscard]] VnlcExportListParsingResult parseExportList();
    [[nodiscard]] VnlcMetadataTermParsingResult parseMetadataTerm();
    [[nodiscard]] VnlcFunctionSignatureParsingResult parseFunctionSignature();
    [[nodiscard]] VnlcTypeParsingResult parseType();
    [[nodiscard]] VnlcParameterParsingResult parseParameter();
    [[nodiscard]] VnlcGenericParameterListParsingResult parseGenericParameterList();
    [[nodiscard]] VnlcGenericArgumentListParsingResult parseGenericArgumentList();
    [[nodiscard]] VnlcImportPathListParsingResult parseImportPathList();
    [[nodiscard]] VnlcFunctionBodyParsingResult parseFunctionBody();
    [[nodiscard]] VnlcClassBodyParsingResult parseClassBody();
    [[nodiscard]] VnlcInterfaceBodyParsingResult parseInterfaceBody();
    [[nodiscard]] VnlcEnumBodyParsingResult parseEnumBody();
    [[nodiscard]] VnlcImportPathItemParsingResult parseImportPathItem();
    [[nodiscard]] VnlcClassMemberParsingResult parseClassMember();
    [[nodiscard]] VnlcConstructorParsingResult parseConstructor(VnlcConstructorParsingContext context);
    [[nodiscard]] VnlcEnumMemberDeclarationParsingResult parseEnumMemberDeclaration();
    [[nodiscard]] VnlcEnumAssociatedValueListParsingResult parseEnumAssociatedValueList();
    [[nodiscard]] VnlcEnumAssociatedValueParsingResult parseEnumAssociatedValue();
    [[nodiscard]] VnlcExpressionParsingResult parseExpression();
    [[nodiscard]] VnlcAssignmentExpressionParsingResult parseAssignmentExpression();
    [[nodiscard]] VnlcConditionalExpressionParsingResult parseConditionalExpression();
    [[nodiscard]] VnlcNullishCoalescingExpressionParsingResult parseNullishCoalescingExpression();
    [[nodiscard]] VnlcLogicalOrExpressionParsingResult parseLogicalOrExpression();
    [[nodiscard]] VnlcLogicalAndExpressionParsingResult parseLogicalAndExpression();
    [[nodiscard]] VnlcEqualityExpressionParsingResult parseEqualityExpression();
    [[nodiscard]] VnlcRelationalExpressionParsingResult parseRelationalExpression();
    [[nodiscard]] VnlcRangeExpressionParsingResult parseRangeExpression();
    [[nodiscard]] VnlcBitwiseOrExpressionParsingResult parseBitwiseOrExpression();
    [[nodiscard]] VnlcBitwiseXorExpressionParsingResult parseBitwiseXorExpression();
    [[nodiscard]] VnlcBitwiseAndExpressionParsingResult parseBitwiseAndExpression();
    [[nodiscard]] VnlcShiftExpressionParsingResult parseShiftExpression();
    [[nodiscard]] VnlcAdditiveExpressionParsingResult parseAdditiveExpression();
    [[nodiscard]] VnlcMultiplicativeExpressionParsingResult parseMultiplicativeExpression();
    [[nodiscard]] VnlcUnaryExpressionParsingResult parseUnaryExpression();
    [[nodiscard]] VnlcExponentialExpressionParsingResult parseExponentialExpression();
    [[nodiscard]] VnlcPostfixExpressionParsingResult parsePostfixExpression();
    [[nodiscard]] VnlcPrimaryExpressionParsingResult parsePrimaryExpression();
    [[nodiscard]] VnlcLiteralParsingResult parseLiteral();
    [[nodiscard]] VnlcStringParsingResult parseString();
    [[nodiscard]] VnlcBooleanParsingResult parseBoolean();
    [[nodiscard]] VnlcListLikeLiteralParsingResult parseListLikeLiteral();
    [[nodiscard]] VnlcDictLiteralParsingResult parseDictLiteral();
    [[nodiscard]] VnlcSelectorParsingResult parseSelector();
    [[nodiscard]] VnlcArgumentListParsingResult parseArgumentList();
    [[nodiscard]] VnlcInterpolationParsingResult parseInterpolation();
    [[nodiscard]] VnlcDictEntryParsingResult parseDictEntry();
    [[nodiscard]] VnlcSelectorArgumentListParsingResult parseSelectorArgumentList();
    [[nodiscard]] VnlcSelectorArgumentParsingResult parseSelectorArgument();
    [[nodiscard]] VnlcStatementParsingResult parseStatement();
    [[nodiscard]] VnlcExpressionStatementParsingResult parseExpressionStatement();
    [[nodiscard]] VnlcVariableDeclarationStatementParsingResult parseVariableDeclarationStatement();
    [[nodiscard]] VnlcBlockStatementParsingResult parseBlockStatement();
    [[nodiscard]] VnlcControlFlowStatementParsingResult parseControlFlowStatement();
    [[nodiscard]] VnlcIfStatementParsingResult parseIfStatement();
    [[nodiscard]] VnlcSwitchStatementParsingResult parseSwitchStatement();
    [[nodiscard]] VnlcWhileStatementParsingResult parseWhileStatement(VnlcWhileStatementParsingContext context);
    [[nodiscard]] VnlcForStatementParsingResult parseForStatement(VnlcForStatementParsingContext context);
    [[nodiscard]] VnlcReturnStatementParsingResult parseReturnStatement();
    [[nodiscard]] VnlcBreakStatementParsingResult parseBreakStatement();
    [[nodiscard]] VnlcContinueStatementParsingResult parseContinueStatement();
    [[nodiscard]] VnlcReloadStatementParsingResult parseReloadStatement();
    [[nodiscard]] VnlcSwitchCaseParsingResult parseSwitchCase();

public:
    explicit VnlcParser(VnlcLexer&& lexer, unsigned int maxBufferSize = 3);

    [[nodiscard]] std::unique_ptr<VnlcModuleNode> parse(const VnlcConfig& config);
};

#endif // VNLC_PARSER_HPP
