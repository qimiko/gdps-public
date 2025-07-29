#pragma once

#ifndef CLASSES_EXTENSIONS_GJCOMMENTEXT_HPP
#define CLASSES_EXTENSIONS_GJCOMMENTEXT_HPP

#include <Geode/Geode.hpp>

enum class CommentBadge : uint16_t {
    GlobalTop50 = 1 << 0,
    GlobalTop10 = 1 << 1,
    GlobalTop1 = 1 << 2,
    CreatorTop50 = 1 << 3,
    CreatorTop10 = 1 << 4,
    CreatorTop1 = 1 << 5,

    Developer = 1 << 11,
    Troll = 1 << 12,
    Coins = 1 << 13,
    Moderator = 1 << 14,
    Admin = 1 << 15,
};
using badge_storage_t = std::underlying_type_t<CommentBadge>;

struct CommentBadgeHash {
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

class GJCommentExt : public cocos2d::CCObject {
public:
    GJCommentExt() : _customColor({ 0xFF, 0xFF, 0xFF }), _hasSetColor(false) {}

    CC_SYNTHESIZE(std::bitset<std::numeric_limits<badge_storage_t>::digits>, _enumBitVal, EnumBitVal);
    CC_SYNTHESIZE(cocos2d::ccColor3B, _customColor, CustomColor);
    CC_SYNTHESIZE(bool, _hasSetColor, HasSetColor);
};

#endif //CLASSES_EXTENSIONS_GJCOMMENTEXT_HPP
