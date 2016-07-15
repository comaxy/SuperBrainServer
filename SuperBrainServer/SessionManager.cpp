#include "SessionManager.h"
#include "StringUtil.h"
#include "LoggerDef.h"
#include "SockEventDef.h"
#include "Application.h"
#include "DbManager.h"
#include "Socket.h"
#include "Player.h"
#include "Session.h"
#include <atlstr.h>

std::shared_ptr<Session> SessionManager::newSession(SOCKET sock)
{
	appLogger()->trace("Creating new session for socket: ", sock);
	m_sessions[sock] = std::make_shared<Session>(sock);
	m_sessions[sock]->initialize();
	appLogger()->trace("New session for socket ", sock, " has been created.", " Now session count is: ", m_sessions.size());
	return m_sessions[sock];
}

std::shared_ptr<Session> SessionManager::findSession(SOCKET sock)
{
	appLogger()->trace("Finding session for socket: ", sock);
	auto iter = m_sessions.find(sock);
	if (iter == m_sessions.end())
	{
		appLogger()->error("Can not find session for socket: ", sock);
		return nullptr;
	}
	appLogger()->trace("Session for socket: ", sock, " has been found.");
	return iter->second;
}

void SessionManager::destorySession(SOCKET sock)
{
	appLogger()->trace("Destorying session for socket: ", sock);
	auto iter = m_sessions.find(sock);
	if (iter != m_sessions.end())
	{
		m_sessions.erase(iter);
		appLogger()->trace("Session for socket: ", sock, " has been destoryed.", " Now session count is: ", m_sessions.size());
	}
	else
	{
		appLogger()->error("Destory session for socket: ", sock, " failed! Reason: Can not find the session.");
	}
}
