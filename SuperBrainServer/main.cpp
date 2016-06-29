#include <Windows.h>
#include "MainWindow.h"
#include "Application.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application::sharedInstance()->setInstanceHandle(hInstance);

	if (!Application::sharedInstance()->initialize())
	{
		return -1;
	}

	// 创建主窗口
	std::shared_ptr<MainWindow> mainWindow = std::make_shared<MainWindow>();
	Application::sharedInstance()->setMainWindow(mainWindow);
	mainWindow->Create(NULL, TEXT("Super Brain"), WS_OVERLAPPED | WS_SYSMENU, 0);
	mainWindow->ShowWindow();

	if (!Application::sharedInstance()->run())
	{
		return -1;
	}
	
	Application::sharedInstance()->uninitialize();

	return 0;
}
