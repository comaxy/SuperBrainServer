#include "MainWindow.h"
#include "Application.h"
#include "SessionManager.h"
#include "Session.h"
#include "LoggerDef.h"
#include "PlayerManager.h"
#include "GameManager.h"
#include "Player.h"

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
		LOG_TRACE("Closing listen socket: ", m_hLstnSock);
		closesocket(m_hLstnSock);
		LOG_TRACE("Listen socket:", m_hLstnSock, " has been closed.");
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
		OnMsgSocketWrite(hSock);
		break;
	case FD_CLOSE:
		OnMsgSocketClose(hSock);
		break;
	default:
		break;
	}
}

void MainWindow::OnMsgSocketAccept(SOCKET sock)
{
	LOG_TRACE("Handle accept event on socket: ", sock);
	if (sock != m_hLstnSock)
	{
		LOG_ERROR("Only listen socket should accept connection. Ignore.");
		return;
	}

	SOCKADDR_IN pstName;
	int nLen = sizeof(pstName);
	SOCKET newSock = accept(sock, (LPSOCKADDR)&pstName, (LPINT)&nLen);
	if (newSock == SOCKET_ERROR)
	{
		LOG_ERROR("Accept incoming socket failed!");
		return;
	}
	else
	{
		LOG_TRACE("New socket accepted. Socket:", newSock);
		Application::sharedInstance()->sessionManager()->newSession(newSock);
	}
}

void MainWindow::OnMsgSocketRead(SOCKET sock)
{
	LOG_TRACE("Handle read event on socket: ", sock);
	std::shared_ptr<Session> session = Application::sharedInstance()->sessionManager()->findSession(sock);
	if (!session)
	{
		LOG_ERROR("Can not find the session for socket ", sock, " for read.");
		return;
	}

	session->socket()->read();
}

void MainWindow::OnMsgSocketWrite(SOCKET sock)
{
	LOG_TRACE("Handle write event on socket: ", sock);
	std::shared_ptr<Session> session = Application::sharedInstance()->sessionManager()->findSession(sock);
	if (!session)
	{
		LOG_ERROR("Can not find the session for socket ", sock, " for write.");
	}

	session->socket()->write();
}

void MainWindow::OnMsgSocketClose(SOCKET sock)
{
	LOG_TRACE("Handle close event on socket: ", sock);
	Application::sharedInstance()->sessionManager()->destorySession(sock);
	std::shared_ptr<Player> player = Application::sharedInstance()->playerManager()->findPlayer(sock);
	if (player != nullptr)
	{
		Application::sharedInstance()->playerManager()->removePlayer(sock);
		Application::sharedInstance()->gameManager()->removeGame(player->gameId());
	}
}

bool MainWindow::OnNotifyWindowInit()
{
	WSADATA wsaData = { 0 };
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		LOG_FATAL("WSAStartup failed!");
		return false;
	}

	SOCKET hLstnSock = socket(AF_INET, SOCK_STREAM, 0);
	if (hLstnSock == INVALID_SOCKET)
	{
		LOG_FATAL("Create listen socket failed!");
		return false;
	}

	LOG_TRACE("Create listen socket succeeded! Socket: ", hLstnSock);

	if (SOCKET_ERROR == WSAAsyncSelect(hLstnSock, m_hWnd, WM_SOCKET, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE))
	{
		LOG_FATAL("WSAAsyncSelect failed!");
		return false;
	}

	SOCKADDR_IN pstSockName = { 0 };
	pstSockName.sin_family = PF_INET;
	pstSockName.sin_port = htons(7062);
	if (SOCKET_ERROR == bind(hLstnSock, (LPSOCKADDR)&pstSockName, sizeof(pstSockName)))
	{
		LOG_FATAL("Bind listen socket failed!");
		return false;
	}

	if (SOCKET_ERROR == listen(hLstnSock, 5))
	{
		LOG_FATAL("Listen on listen socket failed!");
		return false;
	}

	m_hLstnSock = hLstnSock;

	return true;
}
