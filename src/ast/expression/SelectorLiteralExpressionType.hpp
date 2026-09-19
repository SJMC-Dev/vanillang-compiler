#ifndef VNLC_SELECTOR_LITERAL_EXPRESSION_TYPE_HPP
#define VNLC_SELECTOR_LITERAL_EXPRESSION_TYPE_HPP

namespace vnlc {
    enum class SelectorLiteralExpressionType {
        NEAREST_PLAYER,   // @p
        RANDOM_PLAYER,    // @r
        ALL_PLAYERS,      // @a
        ALL_ENTITIES,     // @e
        CURRENT_EXECUTOR, // @s
        NEAREST_ENTITY,   // @n
    };
}

#endif // VNLC_SELECTOR_LITERAL_EXPRESSION_TYPE_HPP