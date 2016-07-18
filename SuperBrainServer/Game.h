#pragma once

#include <atlstr.h>
#include <list>
#include <memory>

class Player;

class Game
{
public:
	Game(const CString& name);
	const CString& id() const { return m_id; }
	bool addPlayer(SOCKET id);
	SOCKET findFriendPlayerId(SOCKET playerId);
	const CString& name() const { return m_name; }

	virtual void start();
	virtual void handleEvent(SOCKET socket, UINT8 eventId, const std::pair<char*, UINT16>& body) = 0;

protected:
	CString m_id;
	std::list<SOCKET> m_players;
	CString m_name;
};
