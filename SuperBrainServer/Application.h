#pragma once

#include <Windows.h>
#include <memory>

class DuiManager;
class DbManager;
class MainWindow;
class SessionManager;
class PlayerManager;
class GameManager;
class Logger;

class Application
{
public:
	static std::shared_ptr<Application> sharedInstance();

	bool initialize();
	bool run();
	void uninitialize();

	void setInstanceHandle(HINSTANCE hInstance) { m_hInstance = hInstance; }
	HINSTANCE instanceHandle() const { return m_hInstance; }
	void setMainWindow(std::shared_ptr<MainWindow> mainWindow) { m_mainWindow = mainWindow; }
	std::shared_ptr<MainWindow> mainWindow() const { return m_mainWindow; }
	std::shared_ptr<SessionManager> sessionManager() const { return m_sessionManager; }
	std::shared_ptr<PlayerManager> playerManager() const { return m_playerManager; }
	std::shared_ptr<GameManager> gameManager() const { return m_gameManager; }
	std::shared_ptr<Logger> logger() const { return m_logger; }
	std::shared_ptr<DbManager> dbManager() const { return m_dbManager; }

private:
	Application();

private:
	static std::shared_ptr<Application> s_instance;

private:
	HINSTANCE m_hInstance;
	std::shared_ptr<DuiManager> m_duiManager;
	std::shared_ptr<DbManager> m_dbManager;
	std::shared_ptr<MainWindow> m_mainWindow;
	std::shared_ptr<SessionManager> m_sessionManager;
	std::shared_ptr<PlayerManager> m_playerManager;
	std::shared_ptr<GameManager> m_gameManager;
	std::shared_ptr<Logger> m_logger;
};
