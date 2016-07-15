#include "Application.h"
#include "DuiManager.h"
#include "DbManager.h"
#include "MainWindow.h"
#include "SessionManager.h"
#include "Logger.h"
#include "LoggerDef.h"
#include <Windows.h>

std::shared_ptr<Application> Application::s_instance;

Application::Application()
	: m_hInstance(NULL)
	, m_duiManager(new DuiManager())
	, m_dbManager(new DbManager())
	, m_mainWindow(nullptr)
	, m_sessionManager(new SessionManager())
	, m_logger(new Logger())
{
}

std::shared_ptr<Application> Application::sharedInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<Application>(new Application());
	}
	return s_instance;
}

bool Application::initialize()
{
	if (!m_logger->initialize())
	{
		return false;
	}

	appLogger()->trace("=========== SUPER BRAIN SERVER ===========");
	appLogger()->trace("Initializing application.");

	if (!m_duiManager->initialize(m_hInstance))
	{
		appLogger()->fatal("Initialize DUI manager failed!");
		return false;
	}

	if (!m_dbManager->initialize())
	{
		appLogger()->fatal("Initialize DB manager failed!");
		return false;
	}

	appLogger()->trace("Initialize application done.");

	return true;
}

bool Application::run()
{
	appLogger()->trace("Application start to run.");

	MSG msg = { 0 };
	BOOL bRet;
	while ((bRet = GetMessage(&msg, *m_mainWindow, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			return false;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	appLogger()->trace("Application stopped.");

	return true;
}

void Application::uninitialize()
{
	appLogger()->trace("Uninitializing application.");

	m_dbManager->uninitialize();
	m_dbManager.reset();
	m_duiManager->uninitialize();
	m_duiManager.reset();

	appLogger()->trace("Uninitialize application done.");

	m_logger->uninitialize();
	m_logger.reset();
}
