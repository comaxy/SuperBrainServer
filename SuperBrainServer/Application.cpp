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
	LOG_TRACE("=========== SUPER BRAIN SERVER ===========");
	LOG_TRACE("Initializing application.");

	if (!m_duiManager->initialize(m_hInstance))
	{
		LOG_FATAL("Initialize DUI manager failed!");
		return false;
	}

	if (!m_dbManager->initialize())
	{
		LOG_FATAL("Initialize DB manager failed!");
		return false;
	}

	LOG_TRACE("Initialize application done.");

	return true;
}

bool Application::run()
{
	LOG_TRACE("Application start to run.");

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

	LOG_TRACE("Application stopped.");

	return true;
}

void Application::uninitialize()
{
	LOG_TRACE("Uninitializing application.");

	m_dbManager->uninitialize();
	m_dbManager.reset();
	m_duiManager->uninitialize();
	m_duiManager.reset();

	LOG_TRACE("Uninitialize application done.");

	m_logger->uninitialize();
	m_logger.reset();
}
