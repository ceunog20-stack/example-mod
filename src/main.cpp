#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <vector>

using namespace geode::prelude;

class $modify(DeathLayoutPlayLayer, PlayLayer) {
	struct Fields {
		bool layoutOn = false;
		std::vector<GameObject*> decos; // 죽을 때 한 번만 모아둠 (매 프레임 전체 순회 방지)
	};

	void hideDecos() {
		for (auto obj : m_fields->decos) {
			obj->setVisible(false);
		}
		if (m_background && Mod::get()->getSettingValue<bool>("hide-bg")) {
			m_background->setVisible(false);
		}
	}

	void setLayoutView(bool on) {
		m_fields->layoutOn = on;

		if (on) {
			m_fields->decos.clear();
			if (m_objects) {
				for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
					// 장식 오브젝트만 (블록, 가시, 오브, 트리거는 유지)
					if (obj && obj->m_objectType == GameObjectType::Decoration) {
						m_fields->decos.push_back(obj);
					}
				}
			}
			this->hideDecos();
		}
		else {
			for (auto obj : m_fields->decos) {
				obj->setVisible(true);
			}
			m_fields->decos.clear();
			if (m_background) {
				m_background->setVisible(true);
			}
		}
	}

	// 죽는 순간 → 레이아웃 ON
	void destroyPlayer(PlayerObject* player, GameObject* obj) {
		PlayLayer::destroyPlayer(player, obj);
		if (!m_fields->layoutOn) {
			this->setLayoutView(true);
		}
	}

	// 게임이 가시성을 다시 계산한 뒤에 장식을 다시 숨김
	void updateVisibility(float dt) {
		PlayLayer::updateVisibility(dt);
		if (m_fields->layoutOn) {
			this->hideDecos();
		}
	}

	// 리스폰 → 원래대로 복구
	void resetLevel() {
		PlayLayer::resetLevel();
		if (m_fields->layoutOn) {
			this->setLayoutView(false);
		}
	}
};
