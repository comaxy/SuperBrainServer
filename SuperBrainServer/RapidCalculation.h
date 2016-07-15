#pragma once

#include "Game.h"
#include <list>

class RapidCalculation : public Game
{
private:
	struct ResultInfo
	{
		SOCKET socket;
		UINT16 result;
		UINT16 time;
	};

public:
	RapidCalculation();
	virtual void start() override;
	virtual void handleEvent(SOCKET socket, UINT8 eventId, const std::pair<char*, UINT16>& body) override;

private:
	void handleResult(SOCKET socket, const std::pair<char*, UINT16>& body);

private:
	UINT16 m_number1;
	UINT16 m_number2;

	std::list<ResultInfo> m_resultInfos;
};
