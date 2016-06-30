#pragma once

#include <Windows.h>
#include <memory>

class DuiManager;
class DbManager;
class MainWindow;
class SessionMgr;
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
	std::shared_ptr<SessionMgr> sessionMgr() const { return m_sessionMgr; }
	std::shared_ptr<Logger> logger() const { return m_logger; }

private:
	Application();

private:
	static std::shared_ptr<Application> s_instance;

private:
	HINSTANCE m_hInstance;
	std::shared_ptr<DuiManager> m_duiManager;
	std::shared_ptr<DbManager> m_dbManager;
	std::shared_ptr<MainWindow> m_mainWindow;
	std::shared_ptr<SessionMgr> m_sessionMgr;
	std::shared_ptr<Logger> m_logger;
};
