#include <Windows.h>
#include <WinSock.h>
#include "UIlib.h"
#include "MainWindow.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 初始化 DuiLib
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	TCHAR szFileName[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	TCHAR resourcePath[MAX_PATH] = { 0 };
	_tcsncpy_s(resourcePath, MAX_PATH, szFileName, 
		_tcsrchr(szFileName, TEXT('\\')) - szFileName + 1);
	_tcscat_s(resourcePath, MAX_PATH, TEXT("skin\\"));
	DuiLib::CPaintManagerUI::SetResourcePath(resourcePath);

	// 创建主窗口
	MainWindow mainWindow;
	mainWindow.Create(NULL, TEXT("Super Brain"), WS_OVERLAPPED | WS_SYSMENU, 0);
	mainWindow.ShowWindow();
	
	MSG msg = { 0 };
	BOOL bRet;
	while ((bRet = GetMessage(&msg, mainWindow, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			return -1;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
