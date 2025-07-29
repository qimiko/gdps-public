#include "classes/managers/secretmanager.hpp"

SecretManager::SecretManager() {}

SecretManager& SecretManager::get() {
	static SecretManager _managerInstance;
	return _managerInstance;
}

void SecretManager::playLevel(int id) {
	if (id == 19) {
		m_inRun = true;
	} else if (!m_canMoveToNext) {
		m_inRun = false;
	} else if (m_currentLevel != id - 1) {
		m_inRun = false;
	}

	m_currentLevel = id;
	m_canMoveToNext = false;
}

void SecretManager::completeLevel(int id) {
	m_canMoveToNext = true;

	if (id == 23 && m_inRun) {
		m_successfulRun = true;
	}
}

bool SecretManager::finishedRun() {
	return m_successfulRun;
}

void SecretManager::clearStatus() {
	m_currentLevel = 0;
	m_inRun = false;
	m_canMoveToNext = false;
	m_successfulRun = false;
}
