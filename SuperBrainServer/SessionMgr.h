#pragma once

#include <atlstr.h>
#include <winsock.h>
#include <memory>
#include <map>

#include "Socket.h"

class Socket;
class Player;

class Session : public SocketReaderDelegate
{
public:
	Session(SOCKET sock);
	bool initialize();

	virtual void readDone(UINT8 eventId, const std::pair<char*, UINT16>& body) override;

	void setFriendName(const CString& friendName) { m_friendName = friendName; }

	std::shared_ptr<Socket> socket() { return m_socket; }
	std::shared_ptr<Player> player() { return m_player; }

private:
	void replyRegister(const CString& body);
	void replyLogin(const CString& body);

private:
	std::shared_ptr<Socket> m_socket;
	std::shared_ptr<Player> m_player;
	CString m_friendName;  // ������֮������������ƣ����ڽ����Ͷ�սʱ����Է��������
};

class SessionMgr
{
public:
	std::shared_ptr<Session> newSession(SOCKET sock);
	std::shared_ptr<Session> findSession(SOCKET sock);
	std::shared_ptr<Session> findSession(const CString& playerName);
	void destorySession(SOCKET sock);
	std::map<SOCKET, std::shared_ptr<Session>> allSessions() const { return m_sessions; }

private:
	std::map<SOCKET, std::shared_ptr<Session>> m_sessions;
};
