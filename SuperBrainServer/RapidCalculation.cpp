#include "RapidCalculation.h"
#include "SockEventDef.h"
#include "SessionManager.h"
#include "Session.h"
#include "Application.h"
#include "StringUtil.h"
#include "Player.h"
#include "PlayerManager.h"
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
	}
}

void RapidCalculation::start()
{
	Game::start();

	srand(static_cast<unsigned int>(time(0)));

	m_number1 = (rand() % 90) + 10;
	m_number2 = (rand() % 90) + 10;

	for (auto playerId : m_players)
	{
		auto session = Application::sharedInstance()->sessionManager()->findSession(playerId);
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
}

void RapidCalculation::handleResult(SOCKET socket, const std::pair<char*, UINT16>& body)
{
	UINT16 calResult = *(UINT16*)body.first;
	UINT16 time = *(UINT16*)(body.first + 2);
	ResultInfo resultInfo;
	resultInfo.socket = socket;
	resultInfo.result = calResult;
	resultInfo.time = time;
	m_resultInfos.push_back(resultInfo);

	bool allDone = true;
	for (auto playerId : m_players)
	{
		if (std::find_if(m_resultInfos.begin(), m_resultInfos.end(),
			[playerId](const ResultInfo& result) { return playerId == result.socket; }) 
			== m_resultInfos.end())
		{
			allDone = false;
		}
	}

	if (allDone)
	{
		CString body;

		for (auto resultInfo : m_resultInfos)
		{
			auto player = Application::sharedInstance()->playerManager()->findPlayer(resultInfo.socket);
			
			body += player->name() + TEXT(";");
			CString resultStr;
			resultStr.Format(TEXT("%ud"), resultInfo.result);
			body += resultStr + TEXT(";");
			CString timeStr;
			timeStr.Format(TEXT("%ud"), resultInfo.time);
			body += timeStr + TEXT(";");
		}

		CString rightResultStr;
		rightResultStr.Format(TEXT("%ud"), m_number1 * m_number2);
		body += rightResultStr + TEXT(";");

		for (auto playerId : m_players)
		{
			auto session = Application::sharedInstance()->sessionManager()->findSession(playerId);

			std::string bodyUtf8 = StringUtil::CStringToUtf8(body);
			UINT16 bodyLength = bodyUtf8.length();
			int bufSize = 3 + bodyLength;
			char* buf = new char[bufSize]();
			UINT8 eventId = RC_FINAL;
			memcpy(buf, (char*)&eventId, 1);
			memcpy(buf + 1, (char*)&bodyLength, 2);
			memcpy(buf + 3, bodyUtf8.c_str(), bodyLength);
			session->socket()->writeBuffer(buf, bufSize);
		}
	}
}
