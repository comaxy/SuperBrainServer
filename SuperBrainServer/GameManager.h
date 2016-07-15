#pragma once

#include <atlstr.h>
#include <map>
#include <memory>

class Game;

class GameManager
{
public:
	std::shared_ptr<Game> newGame(const CString& gameName);
	std::shared_ptr<Game> findGame(const CString& id);
	void removeGame(const CString& id);

private:
	std::map<CString, std::shared_ptr<Game>> m_games;
};
