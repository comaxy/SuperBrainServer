#include <Windows.h>
#include "MainWindow.h"
#include "Application.h"
#include "LoggerDef.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application::sharedInstance()->setInstanceHandle(hInstance);

	if (!Application::sharedInstance()->initialize())
	{
		return -1;
	}

	// 创建主窗口
	LOG_TRACE("Creating main window.");
	std::shared_ptr<MainWindow> mainWindow = std::make_shared<MainWindow>();
	Application::sharedInstance()->setMainWindow(mainWindow);
	HWND hWnd = mainWindow->Create(NULL, TEXT("Super Brain"), WS_OVERLAPPED | WS_SYSMENU, 0);
	if (NULL == hWnd)
	{
		LOG_FATAL("Create main window failed!");
	}
	mainWindow->ShowWindow();

	if (!Application::sharedInstance()->run())
	{
		LOG_FATAL("Run application failed!");
		return -1;
	}
	
	Application::sharedInstance()->uninitialize();

	return 0;
}
