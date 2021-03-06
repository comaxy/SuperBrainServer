#pragma once

#include "Game.h"
#include <map>

class RapidCalculation : public Game
{
private:

	enum State
	{
		PREPARED,
		CALCULATING,
		DONE,
	};

	struct ResultInfo
	{
		State state;
		UINT16 result;
		UINT16 time;
	};

public:
	RapidCalculation();
	virtual void start() override;
	virtual void handleEvent(SOCKET socket, UINT8 eventId, const std::pair<char*, UINT16>& body) override;

private:
	void sendQuestion();
	void handleResult(SOCKET socket, const std::pair<char*, UINT16>& body);
	void handlePrepare(SOCKET socket, const std::pair<char*, UINT16>& body);

private:
	UINT16 m_number1;
	UINT16 m_number2;

	std::map<SOCKET, ResultInfo> m_resultInfos;
};
