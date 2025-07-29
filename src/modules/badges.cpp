#include <Geode/Geode.hpp>
#include <Geode/modify/GJComment.hpp>
#include <Geode/modify/CommentCell.hpp>
#include <Geode/modify/GJUserScore.hpp>
#include <Geode/modify/GJScoreCell.hpp>

#include <vector>
#include <bitset>
#include <limits>
#include <unordered_map>

#include "classes/extensions/gjcommentext.hpp"

std::vector<CommentBadge> bitset_to_badge_enum(const std::bitset<std::numeric_limits<badge_storage_t>::digits> xor_enum)
{
	if (xor_enum == 0) {
		return {};
	}

	auto badge_bit_max = xor_enum.size();
	std::vector<CommentBadge> given_badges;

	for (auto i = 0u; i < badge_bit_max; i++) {
		if (xor_enum.test(i)) {
			given_badges.push_back(static_cast<CommentBadge>(1 << i));
		}
	}

	std::reverse(given_badges.begin(), given_badges.end());

	return given_badges;
}

std::string badge_to_texture(CommentBadge badge)
{
	const std::unordered_map<CommentBadge, std::string, CommentBadgeHash> textures {
		{ CommentBadge::GlobalTop50, "global50Badge.png"_spr },
		{ CommentBadge::GlobalTop10, "global10Badge.png"_spr },
		{ CommentBadge::GlobalTop1, "global01Badge.png"_spr },
		{ CommentBadge::CreatorTop50, "creator50Badge.png"_spr },
		{ CommentBadge::CreatorTop10, "creator10Badge.png"_spr },
		{ CommentBadge::CreatorTop1, "creator01Badge.png"_spr },
		{ CommentBadge::Developer, "developerBadge.png"_spr },
		{ CommentBadge::Troll, "trollBadge.png"_spr },
		{ CommentBadge::Coins, "coinBadge.png"_spr },
		{ CommentBadge::Moderator, "modBadge.png"_spr },
		{ CommentBadge::Admin, "adminBadge.png"_spr },
	};

	if (auto it = textures.find(badge); it != textures.end()) {
		return (*it).second;
	} else {
		return "";
	}
}

struct BadgeGJComment : geode::Modify<BadgeGJComment, GJComment> {
	static GJComment* create(cocos2d::CCDictionary* dict)
	{
		auto comment_obj = GJComment::create(dict);
		auto ext_obj = new GJCommentExt();

		comment_obj->setUserObject("badge"_spr, ext_obj);

		auto badgeValue = dict->valueForKey("11")->uintValue();
		ext_obj->setEnumBitVal(badgeValue);
		ext_obj->setHasSetColor(false);

		auto badgeColors = dict->valueForKey("12");
		if (badgeColors->length() > 0) {
			auto badgeString = badgeColors->getCString();
			auto badgeColors = geode::utils::string::split(badgeString, ",");
			if (badgeColors.size() == 3) {
				auto r = static_cast<uint8_t>(std::atoi(badgeColors[0].c_str()));
				auto g = static_cast<uint8_t>(std::atoi(badgeColors[1].c_str()));
				auto b = static_cast<uint8_t>(std::atoi(badgeColors[2].c_str()));

				ext_obj->setCustomColor({r, g, b});
				ext_obj->setHasSetColor(true);
			}
		}

		if (comment_obj->m_userID == 17414 || comment_obj->m_userID == 16144 || comment_obj->m_userID == 18525) {
			// zylsia or great
			ext_obj->setEnumBitVal(ext_obj->getEnumBitVal().set(12, true));
		}

		return comment_obj;
	}
};

struct BadgeCommentCell : geode::Modify<BadgeCommentCell, CommentCell> {
	void loadFromComment(GJComment* comment) {
		CommentCell::loadFromComment(comment);

		// represents a deleted comment
		if (comment->m_commentID == -1) {
			return;
		}

		if (comment->m_accountID == 0) {
			// for now there's no plans to support whatever that mess is
			return;
		}

		if (auto ext_obj = dynamic_cast<GJCommentExt*>(comment->getUserObject("badge"_spr)); ext_obj != nullptr) {
			auto internal_layer = this->m_mainLayer;
			auto nametag = reinterpret_cast<cocos2d::CCLabelBMFont*>(
				reinterpret_cast<cocos2d::CCMenu*>(
					internal_layer
						->getChildren()
						->objectAtIndex(0))
					->getChildren()
					->objectAtIndex(0));

			auto badge_scale = nametag->getScale() * 0.80f;
			auto current_badge_pos = (24.0f * badge_scale) + (nametag->getContentSize().width * nametag->getScale());

			for (const auto& badge : bitset_to_badge_enum(ext_obj->getEnumBitVal())) {
				auto badge_texture = badge_to_texture(badge);
				if (!badge_texture.empty()) {
					auto badge_sprite = cocos2d::CCSprite::createWithSpriteFrameName(badge_texture.c_str());
					internal_layer->addChild(badge_sprite);
					badge_sprite->setPositionX(current_badge_pos);
					badge_sprite->setPositionY(58.0f);
					badge_sprite->setScale(badge_scale);
					current_badge_pos += 22.0f * badge_scale;
				}
			}

			if (ext_obj->getHasSetColor()) {
				auto glm = GameLevelManager::sharedState();
				if (auto level = glm->getSavedLevel(comment->m_levelID); level != nullptr) {
					if (level->m_userID != comment->m_userID) {
						auto textarea = reinterpret_cast<TextArea*>(internal_layer->getChildren()->objectAtIndex(2));
						textarea->colorAllCharactersTo(ext_obj->getCustomColor());
					}
				}
			}
		}
	}
};

struct BadgeGJUserScore : geode::Modify<BadgeGJUserScore, GJUserScore> {
	static GJUserScore* create(cocos2d::CCDictionary* dict) {
		auto score_obj = GJUserScore::create(dict);
		auto ext_obj = new GJCommentExt();

		score_obj->setUserObject("badge"_spr, ext_obj);

		auto badgeValue = dict->valueForKey("18")->uintValue();
		ext_obj->setEnumBitVal(badgeValue);

		ext_obj->setHasSetColor(false);

		auto badgeColors = dict->valueForKey("19");
		if (badgeColors->length() > 0) {
			auto badgeString = badgeColors->getCString();
			auto badgeColors = geode::utils::string::split(badgeString, ",");
			if (badgeColors.size() == 3) {
				auto r = static_cast<uint8_t>(std::atoi(badgeColors[0].c_str()));
				auto g = static_cast<uint8_t>(std::atoi(badgeColors[1].c_str()));
				auto b = static_cast<uint8_t>(std::atoi(badgeColors[2].c_str()));

				ext_obj->setCustomColor({r, g, b});
				ext_obj->setHasSetColor(true);
			}
		}

		if (score_obj->m_userID == 17414 || score_obj->m_userID == 16144 || score_obj->m_userID == 18525) {
			// zylsia, great or royen
			ext_obj->setEnumBitVal(ext_obj->getEnumBitVal().set(12, true));
		}

		return score_obj;
	}
};

struct BadgeGJScoreCell : geode::Modify<BadgeGJScoreCell, GJScoreCell> {
	void loadFromScore(GJUserScore* score) {
		GJScoreCell::loadFromScore(score);

		if (score->m_accountID == 0) {
			// for now there's no plans to support whatever that mess is
			return;
		}

		if (auto ext_obj = dynamic_cast<GJCommentExt*>(score->getUserObject("badge"_spr)); ext_obj != nullptr) {
			auto internal_layer = this->m_mainLayer;
			auto nametag = reinterpret_cast<cocos2d::CCLabelBMFont*>(
				internal_layer->getChildren()->objectAtIndex(2));

			auto user_badges = bitset_to_badge_enum(ext_obj->getEnumBitVal());
			if (user_badges.size() > 0) {
				auto max_badge_width = 120.0f;
				if (score->m_scoreType == GJScoreType::Creator) {
					max_badge_width = 140.0f;
				}

				auto badge_scale = 0.90f;
				auto nametag_offset = nametag->getPositionX();
				auto badge_offset = (nametag->getContentSize().width * nametag->getScale()) + (12.0f * badge_scale);
				auto badge_padding = 22.0f * badge_scale;

				auto final_badge_width = badge_offset + (badge_padding * user_badges.size());
				if (final_badge_width > max_badge_width) {
					auto scale_factor = max_badge_width / final_badge_width;
					badge_scale = badge_scale * scale_factor;

					nametag->setScale(nametag->getScale() * scale_factor);

					badge_offset = (nametag->getContentSize().width * nametag->getScale()) + (12.0f * badge_scale);
					badge_padding = 22.0f * badge_scale;
				}

				auto current_badge_pos = badge_offset + nametag_offset;

				for (const auto& badge : user_badges) {
					auto badge_texture = badge_to_texture(badge);
					if (!badge_texture.empty()) {
						auto badge_sprite = cocos2d::CCSprite::createWithSpriteFrameName(badge_texture.c_str());
						internal_layer->addChild(badge_sprite);
						badge_sprite->setPositionX(current_badge_pos);
						badge_sprite->setPositionY(23.0f);
						badge_sprite->setScale(badge_scale);
						current_badge_pos += badge_padding;
					}
				}
			}

			if (ext_obj->getHasSetColor()) {
				nametag->setColor(ext_obj->getCustomColor());
			}
		}
	}
};
