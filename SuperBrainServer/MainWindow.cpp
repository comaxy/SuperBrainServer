#include "MainWindow.h"

#define WM_SOCKET (WM_USER + 1000)

DuiLib::CControlUI* MainWindow::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		OnMsgCreate(wParam, lParam);
		break;
	case WM_DESTROY:
		WSACleanup();
		PostQuitMessage(0);
		break;
	case WM_SOCKET:
		OnMsgSocket(wParam, lParam);
		break;
	default:
		break;
	}

	LRESULT lRes = 0;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))
	{
		return lRes;
	}

	return DuiLib::CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void MainWindow::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
	{
		OnNotifyWindowInit();
	}
}

void MainWindow::OnMsgCreate(WPARAM wParam, LPARAM lParam)
{
	m_pm.Init(m_hWnd);

	DuiLib::CDialogBuilder builder;
	DuiLib::CControlUI* pRoot = builder.Create(TEXT("MainWindow.xml"), (UINT)0, this, &m_pm);
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);
}

void MainWindow::OnMsgSocket(WPARAM wParam, LPARAM lParam)
{
	WORD wsaEvent = WSAGETSELECTEVENT(lParam);
	WORD wsaError = WSAGETSELECTERROR(lParam);
	switch (wsaEvent)
	{
	case FD_ACCEPT:
		break;
	case FD_READ:
		break;
	case FD_WRITE:
		break;
	case FD_CLOSE:
		break;
	default:
		break;
	}
}

bool MainWindow::OnNotifyWindowInit()
{
	WSADATA wsaData = { 0 };
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return false;
	}

	SOCKET hLstnSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hLstnSock == INVALID_SOCKET)
	{
		return false;
	}

	if (SOCKET_ERROR == WSAAsyncSelect(hLstnSock, m_hWnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE))
	{
		return false;
	}

	SOCKADDR_IN pstSockName = { 0 };
	pstSockName.sin_family = PF_INET;
	pstSockName.sin_port = htons(7062);
	if (SOCKET_ERROR == bind(hLstnSock, (LPSOCKADDR)&pstSockName, sizeof(pstSockName)))
	{
		return false;
	}

	if (SOCKET_ERROR == listen(hLstnSock, 5))
	{
		return false;
	}

	return true;
}
