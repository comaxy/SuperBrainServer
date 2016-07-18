#include "RapidCalculation.h"
#include "SockEventDef.h"
#include "SessionManager.h"
#include "Session.h"
#include "Application.h"
#include "StringUtil.h"
#include "Player.h"
#include "PlayerManager.h"
#include "LoggerDef.h"
#include <stdio.h>
#include <time.h>
#include <algorithm>

RapidCalculation::RapidCalculation()
	: Game(RAPID_CALCULATION)
{
}

void RapidCalculation::handleEvent(SOCKET socket, UINT8 eventId, const std::pair<char*, UINT16>& body)
{
	switch (eventId)
	{
	case RC_RESULT:
		handleResult(socket, body);
		break;
	case RC_PREPARE:
		handlePrepare(socket, body);
		break;
	default:
		break;
	}
}

void RapidCalculation::start()
{
	Game::start();

	srand(static_cast<unsigned int>(time(0)));

	sendQuestion();
}

void RapidCalculation::sendQuestion()
{
	m_number1 = (rand() % 90) + 10;
	m_number2 = (rand() % 90) + 10;

	for (auto playerId : m_players)
	{
		auto session = Application::sharedInstance()->sessionManager()->findSession(playerId);

		if (session)
		{
			ResultInfo resultInfo;
			resultInfo.state = CALCULATING;
			resultInfo.result = 0;
			resultInfo.time = 0;
			m_resultInfos[playerId] = resultInfo;

			int bufSize = 3 + 16 + 16;
			char* buf = new char[bufSize]();
			UINT8 eventId = RC_START;
			UINT16 bodyLength = 16 + 16;
			memcpy(buf, (char*)&eventId, 1);
			memcpy(buf + 1, (char*)&bodyLength, 2);
			memcpy(buf + 3, (char*)&m_number1, 2);
			memcpy(buf + 5, (char*)&m_number2, 2);
			session->socket()->writeBuffer(buf, bufSize);
		}
		else
		{
			LOG_ERROR("Can not find session of player: ", playerId);
		}
	}
}

void RapidCalculation::handleResult(SOCKET socket, const std::pair<char*, UINT16>& body)
{
	UINT16 calResult = *(UINT16*)body.first;
	UINT16 time = *(UINT16*)(body.first + 2);
	m_resultInfos[socket].result = calResult;
	m_resultInfos[socket].time = time;
	m_resultInfos[socket].state = DONE;

	bool allDone = true;
	for (auto resultInfo : m_resultInfos)
	{
		if (resultInfo.second.state != DONE)
		{
			allDone = false;
		}
	}

	if (allDone)
	{
		CString body;

		for (auto resultInfo : m_resultInfos)
		{
			auto player = Application::sharedInstance()->playerManager()->findPlayer(resultInfo.first);
			
			body += player->name() + TEXT(";");
			CString resultStr;
			resultStr.Format(TEXT("%ud"), resultInfo.second.result);
			body += resultStr + TEXT(";");
			CString timeStr;
			timeStr.Format(TEXT("%ud"), resultInfo.second.time);
			body += timeStr + TEXT(";");
		}

		CString rightResultStr;
		rightResultStr.Format(TEXT("%ud"), m_number1 * m_number2);
		body += rightResultStr + TEXT(";");

		for (auto playerId : m_players)
		{
			auto session = Application::sharedInstance()->sessionManager()->findSession(playerId);
			if (session)
			{
				session->sendStringBody(RC_FINAL, body);
			}
			else
			{
				LOG_ERROR("Can not find session of player: ", playerId);
			}	
		}
	}
}

void RapidCalculation::handlePrepare(SOCKET socket, const std::pair<char*, UINT16>& body)
{
	m_resultInfos[socket].state = PREPARED;

	bool allPrepared = true;
	for (auto resultInfo : m_resultInfos)
	{
		if (resultInfo.second.state != PREPARED)
		{
			allPrepared = false;
		}
	}

	if (allPrepared)
	{
		sendQuestion();
	}
}
