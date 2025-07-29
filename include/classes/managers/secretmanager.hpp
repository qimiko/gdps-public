#pragma once

#ifndef CLASSES_MANAGERS_SECRETMANAGER_HPP
#define CLASSES_MANAGERS_SECRETMANAGER_HPP

class SecretManager {
	SecretManager();

	int m_currentLevel{};
	bool m_inRun{false};
	bool m_canMoveToNext{false};

	bool m_successfulRun{false};

public:
	static SecretManager& get();

	void playLevel(int);
	void completeLevel(int);

	bool finishedRun();
	void clearStatus();

	SecretManager(const SecretManager&) = delete;
	SecretManager(SecretManager&&) = delete;
};

#endif
