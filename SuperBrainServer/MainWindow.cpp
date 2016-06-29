#include "MainWindow.h"
#include "Application.h"
#include "SessionMgr.h"

#define WM_SOCKET (WM_USER + 1000)

MainWindow::MainWindow() : m_pm(), m_hLstnSock(NULL)
{
}

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
	SOCKET hSock = (SOCKET)wParam;
	if (wsaError)
	{
		// handle error
	}

	switch (wsaEvent)
	{
	case FD_ACCEPT:
		OnMsgSocketAccept(hSock);
		break;
	case FD_READ:
		OnMsgSocketRead(hSock);
		break;
	case FD_WRITE:
		break;
	case FD_CLOSE:
		break;
	default:
		break;
	}
}

void MainWindow::OnMsgSocketAccept(SOCKET sock)
{
	if (sock == m_hLstnSock)
	{
		return;
	}

	SOCKADDR_IN pstName;
	int nLen = sizeof(pstName);
	SOCKET newSock = accept(sock, (LPSOCKADDR)&pstName, (LPINT)&nLen);
	if (newSock == SOCKET_ERROR)
	{
		return;
	}
	else
	{
		Application::sharedInstance()->sessionMgr()->newSession(newSock);
	}
}

void MainWindow::OnMsgSocketRead(SOCKET sock)
{
	std::shared_ptr<Session> session = Application::sharedInstance()->sessionMgr()->findSession(sock);
	if (!session)
	{
		return;
	}

	session->setState(Session::READING);
	session->read();
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
