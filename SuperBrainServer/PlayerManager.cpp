#include "PlayerManager.h"
#include "Player.h"

std::shared_ptr<Player> PlayerManager::newPlayer(SOCKET socket, const CString& name)
{
	auto player = std::make_shared<Player>(socket, name);
	m_players[socket] = player;
	return player;
}

std::shared_ptr<Player> PlayerManager::findPlayer(SOCKET id)
{
	if (m_players.find(id) == m_players.end())
	{
		return nullptr;
	}
	return m_players[id];
}

std::shared_ptr<Player> PlayerManager::findPlayer(const CString& name)
{
	for (auto player : m_players)
	{
		if (player.second->name() == name)
		{
			return player.second;
		}
	}
	return nullptr;
}

void PlayerManager::removePlayer(SOCKET id)
{
	auto iter = m_players.find(id);
	if (iter != m_players.end())
	{
		m_players.erase(iter);
	}
}