#include "symbol/FunctionSymbol.hpp"
#include "symbol/RegularSymbol.hpp"
#include <gtest/gtest.h>

namespace vnlc {
    TEST(RegularSymbolTest, StoresKindOriginAccessModifierAndNodes) {
        const RegularSymbol symbol(RegularSymbolKind::PROPERTY, RegularSymbolAccessModifier::PROTECTED, "count", static_cast<const AstNode*>(nullptr));

        EXPECT_EQ(symbol.getKind(), RegularSymbolKind::PROPERTY);
        EXPECT_EQ(symbol.getOrigin(), RegularSymbolOrigin::LOCAL);
        EXPECT_EQ(symbol.getAccessModifier(), RegularSymbolAccessModifier::PROTECTED);
        EXPECT_EQ(symbol.getName(), "count");
        EXPECT_EQ(symbol.getLocalNode(), nullptr);
        EXPECT_EQ(symbol.getImportedNode(), nullptr);
    }

    TEST(FunctionSymbolTest, GroupsOverloadingsByInternalName) {
        FunctionSymbol function("add");
        EXPECT_EQ(function.getName(), "add");
        EXPECT_FALSE(function.isUnique());

        EXPECT_TRUE(
            function.addOverloading(RegularSymbol(RegularSymbolKind::FUNCTION, RegularSymbolAccessModifier::PUBLIC, "__vnl_function_add--int__", static_cast<const AstNode*>(nullptr)))
        );
        EXPECT_TRUE(function.isUnique());

        const auto* integerOverloading = function.getOverloadingByInternalName("__vnl_function_add--int__");
        ASSERT_NE(integerOverloading, nullptr);
        EXPECT_EQ(integerOverloading->getName(), "__vnl_function_add--int__");
        EXPECT_EQ(function.getOverloadingByInternalName("missing"), nullptr);

        EXPECT_TRUE(
            function.addOverloading(RegularSymbol(RegularSymbolKind::FUNCTION, RegularSymbolAccessModifier::PUBLIC, "__vnl_function_add--string__", static_cast<const AstNode*>(nullptr)))
        );
        EXPECT_FALSE(function.isUnique());
        EXPECT_EQ(function.getOverloadings().size(), 2);

        EXPECT_FALSE(
            function.addOverloading(RegularSymbol(RegularSymbolKind::FUNCTION, RegularSymbolAccessModifier::PUBLIC, "__vnl_function_add--int__", static_cast<const AstNode*>(nullptr)))
        );
        EXPECT_EQ(function.getOverloadings().size(), 2);
    }
} // namespace vnlc
