#include "Game.h"
#include "StringUtil.h"

Game::Game(const CString& name) : m_name(name)
{
	m_id = StringUtil::GetGUID();
}

bool Game::addPlayer(SOCKET id)
{
	if (std::find(m_players.begin(), m_players.end(), id) != m_players.end())
	{
		return false;
	}
	m_players.push_back(id);
	return true;
}

SOCKET Game::findFriendPlayerId(SOCKET playerId)
{
	for (auto id : m_players)
	{
		if (id != playerId)
		{
			return id;
		}
	}
	return INVALID_SOCKET;
}
