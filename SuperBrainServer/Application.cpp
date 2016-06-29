#include "Application.h"
#include "DuiManager.h"
#include "DbManager.h"
#include "MainWindow.h"
#include "SessionMgr.h"
#include <Windows.h>

std::shared_ptr<Application> Application::s_instance;

Application::Application()
	: m_hInstance(NULL)
	, m_duiManager(new DuiManager())
	, m_dbManager(new DbManager())
	, m_mainWindow(nullptr)
	, m_sessionMgr(new SessionMgr())
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
	if (!m_duiManager->initialize(m_hInstance))
	{
		return false;
	}

	if (!m_dbManager->initialize())
	{
		return false;
	}

	return true;
}

bool Application::run()
{
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

	return true;
}

void Application::uninitialize()
{
	m_dbManager->uninitialize();
	m_duiManager->uninitialize();
}
