#pragma once

#include <atlstr.h>
#include <winsock.h>
#include <memory>
#include <map>

class Socket;
class Player;
class Session;

class SessionManager
{
public:
	std::shared_ptr<Session> newSession(SOCKET sock);
	std::shared_ptr<Session> findSession(SOCKET sock);
	void destorySession(SOCKET sock);
	std::map<SOCKET, std::shared_ptr<Session>> allSessions() const { return m_sessions; }

private:
	std::map<SOCKET, std::shared_ptr<Session>> m_sessions;
};
