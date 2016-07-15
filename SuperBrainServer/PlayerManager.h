#pragma once

#include <winsock.h>
#include <atlstr.h>
#include <memory>
#include <map>

class Player;

class PlayerManager
{
public:
	std::shared_ptr<Player> newPlayer(SOCKET socket, const CString& name);
	std::shared_ptr<Player> findPlayer(SOCKET id);
	std::shared_ptr<Player> findPlayer(const CString& name);
	void removePlayer(SOCKET id);

private:
	std::map<SOCKET, std::shared_ptr<Player>> m_players;
};
