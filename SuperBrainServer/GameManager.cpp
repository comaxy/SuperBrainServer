#include "GameManager.h"
#include "Game.h"
#include "SockEventDef.h"
#include "RapidCalculation.h"
#include "LoggerDef.h"

std::shared_ptr<Game> GameManager::newGame(const CString& gameName)
{
	if (gameName == RAPID_CALCULATION)
	{
		appLogger()->trace("New game named ", gameName, " has been created.");
		return std::make_shared<RapidCalculation>();
	}
	return nullptr;
}

std::shared_ptr<Game> GameManager::findGame(const CString& id)
{
	if (m_games.find(id) == m_games.end())
	{
		appLogger()->trace("Can not find game ", id);
		return nullptr;
	}
	return m_games[id];
}

void GameManager::removeGame(const CString& id)
{
	auto iter = m_games.find(id);
	if (iter != m_games.end())
	{
		m_games.erase(iter);
		appLogger()->trace("Game ", id, " has been removed.");
	}
	else
	{
		appLogger()->trace("Game ", id, " does not exists.");
	}
}
