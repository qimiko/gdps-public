#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>

#if 0
struct PhysicsPlayLayer : geode::Modify<PhysicsPlayLayer, PlayLayer> {
	void update(float dt) {
		this->m_player->resetCollisionLog();
		if (this->m_dualMode) {
			this->m_player2->resetCollisionLog();
		}

		if (!this->m_player->m_isLocked) {
			this->m_player->setPosition(this->m_player->m_realPlayerPos);
		}

		if (this->m_dualMode && !this->m_player2->m_isLocked) {
			this->m_player2->setPosition(this->m_player2->m_realPlayerPos);
		}

		this->m_player->m_touchedRing = nullptr;
		if (this->m_dualMode) {
			this->m_player2->m_touchedRing = nullptr;
		}

		for (int i = 0; i < this->m_stateObjects->count(); i++) {
			auto obj = static_cast<GameObject*>(this->m_stateObjects->objectAtIndex(i));
			obj->m_stateVar = 0;
		}

		auto frameDuration = dt * 60.0f;
		for (int i = 0; i < this->m_activeObjects->count(); i++) {
			auto obj = static_cast<GameObject*>(this->m_activeObjects->objectAtIndex(i));
			obj->update(frameDuration);
		}

		auto stepCount = std::max(static_cast<int>(std::round(frameDuration * 4.0f)), 4);

		this->m_player->m_lastPos = this->m_player->getPosition();
		if (this->m_dualMode) {
			this->m_player2->m_lastPos = this->m_player2->getPosition();
		}

		if (!this->m_isDead) {
			auto stepFactor = frameDuration / stepCount;
			for (auto step = 0; step < stepCount; step++) {
				auto canJump = true;
				if (this->m_dualMode && this->m_player->m_rollMode && this->m_player2->m_rollMode) {
					canJump = this->m_player->m_canJump && this->m_player2->m_canJump;
				}

				this->m_player->m_allowRollJump = canJump;
				this->m_player2->m_allowRollJump = canJump;

				this->m_levelTime += dt / stepCount;

				if (!this->m_player1PushSaved) {
					if (this->m_player1ReleaseSaved) {
						this->saveRecordAction(false, this->m_player);
					}
				} else {
					this->saveRecordAction(true, this->m_player);
				}

				if (!this->m_player2PushSaved) {
					if (this->m_player2ReleaseSaved) {
						this->saveRecordAction(false, this->m_player2);
					}
				} else {
					this->saveRecordAction(true, this->m_player2);
				}

				this->updateReplay(static_cast<float>(this->m_levelTime));

				auto wasDual = this->m_dualMode;

				this->m_player->update(stepFactor);
				this->checkCollisions(this->m_player, stepFactor);

				if (wasDual && this->m_dualMode) {
					this->m_player2->update(stepFactor);
					this->checkCollisions(this->m_player2, stepFactor);
				}

				if (!this->m_isDead) {
					this->m_player->updateCheckpointTest();
				}

				if (this->m_isDead)
					break;
			}
		}

		this->m_player->updateRotation(frameDuration);
		if (this->m_dualMode) {
			this->m_player2->updateRotation(frameDuration);
		}

		auto isDualBall = this->m_dualMode && this->m_player->m_rollMode && this->m_player2->m_rollMode;
		if (isDualBall && !this->m_levelSettings->m_twoPlayerMode) {
			if (this->m_player->m_gravityFlipped == this->m_player2->m_gravityFlipped) {
				auto playerBox = this->m_player->getScale() * 30.0f * 0.5f;
				auto player2Box = this->m_player2->getScale() * 30.0f * 0.5f;

				auto playerDist = std::abs(this->m_player->getPosition().y - this->m_player2->getPosition().y);
				if (playerDist < playerBox + player2Box + 5.0f && (this->m_player->m_onGround || this->m_player2->m_onGround)) {
					auto actingPlayer = this->m_player;
					auto actedPlayer = this->m_player2;
					auto actingPlayerSize = playerBox;

					if (!actingPlayer->m_onGround && actedPlayer->m_onGround) {
						actedPlayer = this->m_player;
						actingPlayer = this->m_player2;
						actingPlayerSize = player2Box;
					}

					actedPlayer->hardFlipGravity();

					auto ringColor = actingPlayer->m_glowColor1;
					auto ringPosition = actingPlayer->getPosition();

					auto ring = CCCircleWave::create(actingPlayerSize + 2.0f, actingPlayerSize * 4.0f, 0.3f, false, false);
					ring->m_lineWidth = 4;
					ring->m_color = ringColor;
					ring->m_position = ringPosition;

					this->m_gameLayer->addChild(ring, 0);
					ring->m_circleMode = CircleMode::Outlined;
					ring->followObject(actingPlayer, false);
				}
			}
		}

		for (int i = 0; i < this->m_stateObjects->count(); i++) {
			auto obj = static_cast<GameObject*>(this->m_stateObjects->objectAtIndex(i));
			obj->updateState();
		}

		bool inFlip = false;
		cocos2d::CCPoint playerFlipPos;
		cocos2d::CCPoint player2FlipPos;
		if (!this->m_player->m_isLocked) {
			this->m_player->m_realPlayerPos = this->m_player->getPosition();

			if (this->m_dualMode) {
				this->m_player2->m_realPlayerPos = this->m_player2->getPosition();
			}

			auto flipValue = this->m_flipValue;
			if (flipValue != 0.0f && flipValue != 1.0f) {
				if (this->m_cameraFlip == -1.0f) {
					flipValue = 1.0f - flipValue;
				}

				cocos2d::CCPoint flipOffset = {flipValue * 150.0f, 0.0f};
				playerFlipPos = this->m_player->getPosition() + flipOffset;

				if (this->m_dualMode) {
					player2FlipPos = this->m_player2->getPosition() + flipOffset;
				}

				inFlip = true;
			}
		}

		this->updateCamera(frameDuration);
		this->updateVisibility();
		this->checkSpawnObjects();

		this->m_clkTimer += static_cast<double>(dt);

		this->m_player->m_clkTimer = this->m_clkTimer;
		this->m_player2->m_clkTimer = this->m_clkTimer;

		if (!this->m_meteringEnabled) {
			this->m_audioEffectsLayer->audioStep(dt);
		}

		this->updateLevelColors();
		if (inFlip) {
			this->m_player->setPosition(playerFlipPos);
			if (this->m_dualMode) {
				this->m_player2->setPosition(player2FlipPos);
			}
		}

		this->updateProgressbar();
		this->updateEffectPositions();
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("PlayLayer::update", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for PlayLayer::update");
		}
	}
};
#endif

struct PhysicsGameObject : geode::Modify<PhysicsGameObject, GameObject> {
	void customSetup() {
		// the python script i used to generate this is shorter than the table itself
		// this normalizes hitboxes to be based on high graphics (most lenient and also the most noticeable)
		constexpr std::array<std::pair<float, float>, 744> fixed_hitboxes {{
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 26.5f},
			{24.0f, 75.0f},
			{24.0f, 75.0f},
			{33.0f, 85.0f},
			{33.0f, 85.0f},
			{0.0f, 0.0f},
			{6.5f, 42.0f},
			{6.0f, 26.5f},
			{5.5f, 12.5f},
			{128.0f, 41.5f},
			{104.0f, 36.5f},
			{0.0f, 0.0f},
			{42.0f, 12.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 13.5f},
			{30.0f, 13.5f},
			{19.5f, 69.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{43.5f, 90.5f},
			{43.5f, 90.5f},
			{33.0f, 85.0f},
			{115.0f, 38.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 28.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{27.5f, 30.0f},
			{30.0f, 17.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{14.5f, 14.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 15.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.5f, 30.0f},
			{30.0f, 30.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{53.5f, 54.5f},
			{37.5f, 37.5f},
			{85.5f, 85.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{25.0f, 25.5f},
			{39.0f, 39.5f},
			{32.0f, 89.0f},
			{0.0f, 0.0f},
			{31.75f, 89.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{27.0f, 65.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{19.5f, 34.0f},
			{33.0f, 85.0f},
			{25.0f, 77.5f},
			{124.5f, 39.5f},
			{86.0f, 33.5f},
			{55.5f, 26.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{27.5f, 30.0f},
			{17.5f, 31.5f},
			{0.0f, 0.0f},
			{17.5f, 20.0f},
			{12.5f, 17.5f},
			{47.0f, 46.5f},
			{79.0f, 31.5f},
			{114.5f, 44.0f},
			{0.0f, 0.0f},
			{46.5f, 33.5f},
			{14.0f, 46.5f},
			{0.0f, 0.0f},
			{46.0f, 49.5f},
			{0.0f, 0.0f},
			{80.0f, 79.5f},
			{51.0f, 50.5f},
			{27.5f, 27.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.25f, 30.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.25f, 30.0f},
			{30.25f, 13.5f},
			{27.5f, 27.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{15.5f, 30.0f},
			{9.0f, 19.0f},
			{0.0f, 0.0f},
			{47.0f, 41.0f},
			{9.5f, 39.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 24.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 21.5f},
			{30.0f, 21.5f},
			{30.0f, 21.5f},
			{0.0f, 0.0f},
			{29.5f, 21.5f},
			{0.0f, 0.0f},
			{13.5f, 21.5f},
			{30.25f, 30.0f},
			{30.0f, 15.5f},
			{20.25f, 20.0f},
			{53.75f, 53.75f},
			{36.25f, 36.0f},
			{24.5f, 24.5f},
			{0.0f, 0.0f},
			{60.0f, 52.0f},
			{9.5f, 40.0f},
			{84.25f, 84.5f},
			{60.75f, 60.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 21.5f},
			{0.0f, 0.0f},
			{21.5f, 21.5f},
			{0.0f, 0.0f},
			{15.0f, 7.5f},
			{22.0f, 21.5f},
			{30.0f, 13.5f},
			{0.0f, 0.0f},
			{35.0f, 43.5f},
			{33.0f, 56.5f},
			{50.5f, 56.5f},
			{65.0f, 56.5f},
			{15.25f, 7.5f},
			{30.0f, 13.5f},
			{15.25f, 15.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.25f, 0.25f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 13.5f},
			{0.0f, 0.0f},
			{30.0f, 13.5f},
			{0.0f, 0.0f},
			{15.0f, 7.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{94.5f, 94.5f},
			{57.0f, 63.5f},
			{0.0f, 0.0f},
			{30.0f, 15.5f},
			{23.0f, 23.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{22.5f, 22.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{12.5f, 30.0f},
			{21.5f, 21.5f},
			{30.0f, 21.5f},
			{0.0f, 0.0f},
			{12.5f, 21.5f},
			{0.0f, 0.0f},
			{30.0f, 17.5f},
			{30.0f, 16.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.25f, 0.25f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.25f, 0.25f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.25f, 0.25f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.25f, 0.25f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 22.5f},
			{0.0f, 0.0f},
			{12.5f, 15.0f},
			{40.5f, 89.5f},
			{40.5f, 89.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{11.0f, 12.5f},
			{29.0f, 13.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{21.5f, 21.5f},
			{42.5f, 21.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 14.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 9.5f},
			{30.0f, 13.5f},
			{30.0f, 13.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{13.5f, 13.5f},
			{27.0f, 13.5f},
			{25.0f, 67.5f},
			{21.5f, 54.0f},
			{18.5f, 39.5f},
			{13.5f, 26.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{12.5f, 12.0f},
			{12.5f, 12.0f},
			{80.5f, 70.0f},
			{48.5f, 42.0f},
			{22.5f, 19.5f},
			{0.0f, 0.0f},
			{54.5f, 63.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{13.5f, 14.0f},
			{11.0f, 8.5f},
			{5.5f, 6.0f},
			{0.0f, 0.0f},
			{21.5f, 21.5f},
			{30.0f, 21.5f},
			{0.0f, 0.0f},
			{13.0f, 21.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 25.5f},
			{30.0f, 22.5f},
			{0.0f, 0.0f},
			{30.0f, 10.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{25.0f, 18.0f},
			{30.0f, 24.5f},
			{23.5f, 20.5f},
			{47.5f, 60.0f},
			{36.0f, 38.5f},
			{25.5f, 21.5f},
			{0.0f, 0.0f},
			{21.5f, 21.5f},
			{30.0f, 21.5f},
			{0.0f, 0.0f},
			{13.0f, 21.5f},
			{12.5f, 12.0f},
			{12.5f, 12.0f},
			{28.5f, 44.0f},
			{27.5f, 30.0f},
			{17.5f, 31.5f},
			{0.0f, 0.0f},
			{17.5f, 20.0f},
			{12.5f, 17.5f},
			{47.0f, 46.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{1.5f, 1.5f},
			{2.5f, 2.5f},
			{3.5f, 2.0f},
			{30.0f, 1.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{23.5f, 12.5f},
			{30.0f, 15.5f},
			{23.5f, 23.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{30.0f, 1.5f},
			{15.0f, 1.5f},
			{11.5f, 11.5f},
			{16.5f, 16.5f},
			{11.5f, 11.5f},
			{0.0f, 0.0f},
			{61.5f, 31.5f},
			{31.5f, 31.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{33.0f, 85.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{7.5f, 7.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{80.0f, 80.5f},
			{51.5f, 52.5f},
			{0.0f, 0.0f},
			{79.5f, 79.5f},
			{51.5f, 54.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{25.0f, 25.0f},
			{55.0f, 27.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{25.0f, 25.0f},
			{55.0f, 27.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{22.5f, 22.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{30.0f, 9.5f},
			{7.5f, 7.5f},
			{9.0f, 9.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{84.25f, 84.5f},
			{60.75f, 60.5f},
			{0.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 0.0f}
		}};

		if (m_objectID >= 0 && m_objectID <= fixed_hitboxes.size()) {
			auto [width, height] = fixed_hitboxes[m_objectID - 1];
			if (width != 0.0f || height != 0.0f) {
				m_orientedBoxHeight = height;
				m_orientedBoxWidth = width;
			}
		}

		GameObject::customSetup();
	}
};
