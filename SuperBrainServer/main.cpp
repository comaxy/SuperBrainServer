#include <Windows.h>
#include <WinSock.h>
#include <atlstr.h>
#include "UIlib.h"
#include "MainWindow.h"
#include "sqlite/sqlite3.h"
#include "StringUtil.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 初始化 DuiLib
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	CString exeFilePath;
	GetModuleFileName(NULL, exeFilePath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	CString resourcePath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("skin\\");
	DuiLib::CPaintManagerUI::SetResourcePath(static_cast<LPCTSTR>(resourcePath));

	//// 初始化数据库
	//CString dbPath = exeFilePath.Left(exeFilePath.ReverseFind(TEXT('\\')) + 1) + TEXT("sb.db");
	//sqlite3* db = NULL;
	//sqlite3_open(StringUtil::CStringToUtf8(dbPath).c_str(), &db);

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
