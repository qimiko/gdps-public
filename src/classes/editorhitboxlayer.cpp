#include "classes/editorhitboxlayer.hpp"

bool EditorHitboxLayer::init() {
	this->_hitboxNode = cocos2d::CCDrawNode::create();
	this->addChild(this->_hitboxNode);

	return true;
}

void EditorHitboxLayer::beginUpdate() {
	// clear the hitbox node so we can add new things
	this->_hitboxNode->clear();
}

std::array<cocos2d::CCPoint, 4> boundingBoxToPoints(OBB2D* box) {
	std::array<cocos2d::CCPoint, 4> points{{
		box->m_obVertexBottomLeft,
		box->m_obVertexBottomRight,
		box->m_obVertexTopRight,
		box->m_obVertexTopLeft
	}};

	return points;
}

void EditorHitboxLayer::drawPlayer(PlayerObject* player) {
	// this magic number comes thanks to idk
	// like literally idk please tell me where it came from thanks
	auto rect = player->getObjectRect(0.3f, 0.3f);

	std::array<cocos2d::CCPoint, 4> points{{
		{ rect.getMinX(), rect.getMinY() },
		{ rect.getMaxX(), rect.getMinY() },
		{ rect.getMaxX(), rect.getMaxY() },
		{ rect.getMinX(), rect.getMaxY() }
	}};

	// hitbox for player -> block collisions
	this->_hitboxNode->drawPolygon(
		points.data(), 4,
		{ 0.0f, 0.0f, 0.0f, 0.0f }, HITBOX_WIDTH,
		// hopefully this helps contrast against the big x
		{ 1.0f, 1.0f, 1.0f, 1.0f }
	);

	if (_skipHitboxUpdates && !player->m_objectOBB2D) {
		return;
	}

	auto box = player->getOrientedBox();

	if (!_skipHitboxUpdates) {
		player->updateOrientedBox();
	}

	std::array<cocos2d::CCPoint, 4> bound_points = boundingBoxToPoints(box);
	this->_hitboxNode->drawPolygon(
		bound_points.data(), 4,
		{ 0.0f, 0.0f, 0.0f, 0.0f }, HITBOX_WIDTH,
		{ 0.0f, 0.0f, 1.0f, 1.0f }
	);

	// draw unoriented hitbox
	auto player_rect = player->getObjectRect();
	std::array<cocos2d::CCPoint, 4> player_points{{
		{ player_rect.getMinX(), player_rect.getMinY() },
		{ player_rect.getMaxX(), player_rect.getMinY() },
		{ player_rect.getMaxX(), player_rect.getMaxY() },
		{ player_rect.getMinX(), player_rect.getMaxY() }
	}};
	this->_hitboxNode->drawPolygon(
		player_points.data(), 4,
		{ 0.0f, 0.0f, 0.0f, 0.0f }, HITBOX_WIDTH,
		{ 0.0f, 0.0f, 0.0f, 1.0f }
	);
}

void EditorHitboxLayer::drawSlope(GameObject* obj, cocos2d::ccColor4F& color) {
	// update slope type

	if (!_skipHitboxUpdates) {
		obj->determineSlopeDirection();
	}

	std::array<cocos2d::CCPoint, 3> slope_points {{}};

	auto rect = obj->getObjectRect();

	auto third_point_pos = rect.getMinX();

	// determine where the flat edges of the slope are
	if (obj->slopeWallLeft()) {
		// bottom left and top left are unique
		slope_points[0] = cocos2d::CCPoint(rect.getMinX(), rect.getMinY());
		slope_points[1] = cocos2d::CCPoint(rect.getMinX(), rect.getMaxY());
		third_point_pos = rect.getMaxX();
	} else {
		// bottom right and top right are unique
		slope_points[0] = cocos2d::CCPoint(rect.getMaxX(), rect.getMinY());
		slope_points[1] = cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY());
		third_point_pos = rect.getMinX();
	}

	if (obj->slopeFloorTop()) {
		// floor is on top, third point is on top
		slope_points[2] = cocos2d::CCPoint(third_point_pos, rect.getMaxY());
	} else {
		// otherwise it's on bottom
		slope_points[2] = cocos2d::CCPoint(third_point_pos, rect.getMinY());
	}

	if (obj->m_hazardousSlope) {
		auto slopeMod = cocos2d::CCPoint {0.0f, obj->slopeFloorTop() ? -4.0f : 4.0f};
		auto alpha = color.a;

		if (obj->slopeFloorTop()) {
			this->_hitboxNode->drawSegment(slope_points[0] - slopeMod, slope_points[2] - slopeMod, HITBOX_WIDTH, { 1.0f, 0.0f, 0.0f, alpha });
		} else {
			this->_hitboxNode->drawSegment(slope_points[1] + slopeMod, slope_points[2] + slopeMod, HITBOX_WIDTH, { 1.0f, 0.0f, 0.0f, alpha });
		}
	}

	this->_hitboxNode->drawPolygon(slope_points.data(), 3, { 0.0f, 0.0f, 0.0f, 0.0f }, HITBOX_WIDTH, color);
}

void EditorHitboxLayer::drawCircle(GameObject* obj, const cocos2d::ccColor4F& color) {
	// draw circle
	auto radius = obj->m_objectRadius;
	std::array<cocos2d::CCPoint, 32> points{{}};
	auto pointsCount = points.max_size();

	auto coef = 2.0 * M_PI/pointsCount;
	auto center = obj->getPosition();

	for (auto i = 0u; i < pointsCount; i++) {
		auto rads = coef * i;
		auto x = static_cast<float>(radius * cos(rads) + center.x);
		auto y = static_cast<float>(radius * sin(rads) + center.y);
		points.at(i) = cocos2d::CCPoint(x, y);
	}

	this->_hitboxNode->drawPolygon(points.data(), pointsCount, { 0.0f, 0.0f, 0.0f, 0.0f }, HITBOX_WIDTH, color);

}

void EditorHitboxLayer::drawObject(GameObject* obj, int groupFilter) {
	cocos2d::ccColor4F color = { 0.0f, 0.0f, 0.0f, 0.0f };
	switch (obj->m_objectType) {
		case GameObjectType::Solid:
		case GameObjectType::Basic:
		case GameObjectType::Slope:
			color = { 0.0f, 0.0f, 1.0f, 1.0f };
			break;
		case GameObjectType::Hazard:
			color = { 1.0f, 0.0f, 0.0f, 1.0f };
			break;
		case GameObjectType::Breakable:
		case GameObjectType::SecretCoin:
			if (obj->m_sleeping) {
				// assume object has been collected, thus no collisions
				return;
			}
			color = { 1.0f, 0.0f, 1.0f, 1.0f };
			break;
		case GameObjectType::YellowJumpPad:
		case GameObjectType::PinkJumpPad:
		case GameObjectType::GravityPad:
		case GameObjectType::YellowJumpRing:
		case GameObjectType::PinkJumpRing:
		case GameObjectType::GravityRing:
			color = { 0.0f, 1.0f, 1.0f, 1.0f };
			break;
		case GameObjectType::Modifier:
		case GameObjectType::InverseGravityPortal:
		case GameObjectType::NormalGravityPortal:
		case GameObjectType::NormalMirrorPortal:
		case GameObjectType::InverseMirrorPortal:
		case GameObjectType::RegularSizePortal:
		case GameObjectType::MiniSizePortal:
		case GameObjectType::DualPortal:
		case GameObjectType::SoloPortal:
		case GameObjectType::CubePortal:
		case GameObjectType::ShipPortal:
		case GameObjectType::UfoPortal:
		case GameObjectType::BallPortal:
		case GameObjectType::WavePortal:
			color = { 0.0f, 1.0f, 0.0f, 1.0f };
			break;
		case GameObjectType::PulsingDecoration:
		case GameObjectType::Decoration:
		default:
			// object has no collisions to care about
			return;
	}

	if (groupFilter != -1 && obj->m_editorLayer != groupFilter) {
		color.a = 0.2f;
	}

	if (obj->m_isSelected) {
		// make selected objects more obvious
		color = { 0.0f, 1.0f, 0.0f, color.a };
	}

	if (auto radius = obj->m_objectRadius; radius > 0.0f) {
		this->drawCircle(obj, color);
		return;
	}

	if (obj->m_objectType == GameObjectType::Slope) {
		this->drawSlope(obj, color);
		return;
	}

	if (obj->m_oriented) {
		if (_skipHitboxUpdates && !obj->m_objectOBB2D) {
			return;
		}

		auto box = obj->getOrientedBox();
		// this should only be run when the object is actually modified. make that the case
		if (!_skipHitboxUpdates) {
			obj->updateOrientedBox();
		}

		std::array<cocos2d::CCPoint, 4> bound_points = boundingBoxToPoints(box);
		this->_hitboxNode->drawPolygon(bound_points.data(), 4, { 0.0f, 0.0f, 0.0f, 0.0f }, HITBOX_WIDTH, color);
		return;
	}

	auto rect = obj->getObjectRect();
	std::array<cocos2d::CCPoint, 4> points{{
		{ rect.getMinX(), rect.getMinY() },
		{ rect.getMaxX(), rect.getMinY() },
		{ rect.getMaxX(), rect.getMaxY() },
		{ rect.getMinX(), rect.getMaxY() }
	}};
	this->_hitboxNode->drawPolygon(points.data(), 4, { 0.0f, 0.0f, 0.0f, 0.0f }, HITBOX_WIDTH, color);
}

void EditorHitboxLayer::drawVerticalLine(float x) {
	this->_hitboxNode->drawSegment(
		cocos2d::CCPoint(x, 1000.0f),
		cocos2d::CCPoint(x, 0.0f),
		1.0f, {1.0f, 1.0f, 1.0f, 1.0f}
	);
}