#include "SessionMgr.h"
#include "StringUtil.h"
#include "LoggerDef.h"
#include "SockEventDef.h"
#include "Application.h"
#include "DbManager.h"
#include <atlstr.h>

void SocketReader::read()
{
	switch (m_state)
	{
	case READING_HEADER:
		{
			appLogger()->trace("Socket ", m_sock, " reading header.");
			int recvSize = recv(m_sock, m_readPos, m_remainSize, 0);
			m_remainSize -= recvSize;
			if (m_remainSize == 0)
			{
				m_state = READING_BODY;
				m_bodySize = *(reinterpret_cast<UINT16*>(&m_header[1]));
				m_body = new char[m_bodySize]();
				m_readPos = m_body;
				m_remainSize = m_bodySize;

				appLogger()->trace("Socket ", m_sock, " read header done.");
			}
			else
			{
				m_state = READING_HEADER;
				m_readPos += recvSize;
			}
		}
		break;
	case READING_BODY:
		{
			appLogger()->trace("Socket ", m_sock, " reading body.");
			int recvSize = recv(m_sock, m_readPos, m_remainSize, 0);
			m_remainSize -= recvSize;
			if (m_remainSize == 0)
			{
				m_state = READ_DONE;
				appLogger()->trace("Socket ", m_sock, " read body done.");
				appLogger()->trace("=============== EVENT RECEIVED DONE ===============");
				appLogger()->trace("Event Id: ", *(UINT8*)&m_header[0], ", Body Length: ", m_bodySize);
				appLogger()->trace("===================================================");
				if (m_delegate)
				{
					m_delegate->readDone(*(UINT8*)&m_header[0], std::make_pair(m_body, m_bodySize));
				}
			}
			else
			{
				m_state = READING_BODY;
				m_readPos += recvSize;
			}
		}
		break;
	case READ_DONE:
		break;
	default:
		break;
	}
}

void Session::read()
{
	m_reader.read();
}

void Session::readDone(UINT8 eventId, const std::pair<char*, UINT16>& body)
{
	switch (eventId)
	{
	case REGISTER:
		{
			std::string utf8Str(body.first, body.second);
			CString playerInfo = StringUtil::Utf8ToCString(utf8Str);
			int seperatorPos = playerInfo.Find(TEXT(';'));
			CString playerName = playerInfo.Left(seperatorPos);
			CString password = playerInfo.Mid(seperatorPos + 1);
			std::string playerNameMb = StringUtil::CStringToMultiByte(playerName);
			appLogger()->trace("Handle socket ", m_sock, " register event. ",
				"Player name: ", playerNameMb.c_str());
			if (playerName.IsEmpty())
			{
				appLogger()->trace("Error: Player name for register is empty! Socket: ", m_sock);
				return;
			}
			if (password.IsEmpty())
			{
				appLogger()->trace("Error: Player password for register is empty! Socket: ", m_sock);
				return;
			}

			auto query = Application::sharedInstance()->dbManager()->sqlite()->makeQuery();
			query->prepare("SELECT * FROM player WHERE name=:name");
			query->bindValue(TEXT(":name"), playerName);
			query->exec();
			if (query->next())
			{
				appLogger()->trace("Register failed! Player named ", playerNameMb.c_str(), " has already exists. Socket: ", m_sock);
			}
			else
			{
				query->prepare("INSERT INTO player(name,password) VALUES(:name,:password)");
				query->bindValue(TEXT(":name"), playerName);
				query->bindValue(TEXT(":password"), password);
				query->exec();
				appLogger()->trace("Register succeeded! Socket: ", m_sock);
			}
		}
		break;
	case LOGIN:
		{
			std::string utf8Str(body.first, body.second);
			CString playerInfo = StringUtil::Utf8ToCString(utf8Str);
			int seperatorPos = playerInfo.Find(TEXT(';'));
			CString playerName = playerInfo.Left(seperatorPos);
			CString password = playerInfo.Mid(seperatorPos + 1);
			std::string playerNameMb = StringUtil::CStringToMultiByte(playerName);
			appLogger()->trace("Handle socket ", m_sock, " login event. ", 
				"Player name: ", playerNameMb.c_str());
			if (playerName.IsEmpty())
			{
				appLogger()->trace("Error: Player name for login is empty! Socket: ", m_sock);
				return;
			}
			if (password.IsEmpty())
			{
				appLogger()->trace("Error: Player password for login is empty! Socket: ", m_sock);
				return;
			}
			m_playerName = playerName;
		}
		break;
	default:
		break;
	}
}

bool Session::initialize()
{
	m_reader.setDelegate(this);
	return true;
}

std::shared_ptr<Session> SessionMgr::newSession(SOCKET sock)
{
	appLogger()->trace("Creating new session for socket: ", sock);
	m_sessions[sock] = std::make_shared<Session>(sock);
	m_sessions[sock]->initialize();
	appLogger()->trace("New session for socket ", sock, " has been created.", " Now session count is: ", m_sessions.size());
	return m_sessions[sock];
}

std::shared_ptr<Session> SessionMgr::findSession(SOCKET sock)
{
	appLogger()->trace("Finding session for socket: ", sock);
	auto iter = m_sessions.find(sock);
	if (iter == m_sessions.end())
	{
		appLogger()->error("Can not find session for socket: ", sock);
		return nullptr;
	}
	appLogger()->trace("Session for socket: ", sock, " has been found.");
	return iter->second;
}

void SessionMgr::destorySession(SOCKET sock)
{
	appLogger()->trace("Destorying session for socket: ", sock);
	auto iter = m_sessions.find(sock);
	if (iter != m_sessions.end())
	{
		m_sessions.erase(iter);
		appLogger()->trace("Session for socket: ", sock, " has been destoryed.", " Now session count is: ", m_sessions.size());
	}
	else
	{
		appLogger()->error("Destory session for socket: ", sock, " failed! Reason: Can not find the session.");
	}
}
