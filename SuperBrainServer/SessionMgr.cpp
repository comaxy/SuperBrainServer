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
				appLogger()->trace("Socket ", m_sock, " read header done.");

				m_state = READING_BODY;
				m_bodySize = *(reinterpret_cast<UINT16*>(&m_header[1]));
				if (m_bodySize > 0)
				{
					m_body = new char[m_bodySize]();
					m_readPos = m_body;
					m_remainSize = m_bodySize;
				}
				else
				{
					appLogger()->trace("Socket ", m_sock, " read body done.");
					appLogger()->trace("=============== EVENT RECEIVED DONE ===============");
					appLogger()->trace("Event Id: ", *(UINT8*)&m_header[0], ", Body Length: ", m_bodySize);
					appLogger()->trace("===================================================");
					if (m_delegate)
					{
						m_delegate->readDone(*(UINT8*)&m_header[0], std::make_pair(m_body, m_bodySize));
					}
					clear();
				}
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
				appLogger()->trace("Socket ", m_sock, " read body done.");
				appLogger()->trace("=============== EVENT RECEIVED DONE ===============");
				appLogger()->trace("Event Id: ", *(UINT8*)&m_header[0], ", Body Length: ", m_bodySize);
				appLogger()->trace("===================================================");
				if (m_delegate)
				{
					m_delegate->readDone(*(UINT8*)&m_header[0], std::make_pair(m_body, m_bodySize));
				}
				clear();
			}
			else
			{
				m_state = READING_BODY;
				m_readPos += recvSize;
			}
		}
		break;
	default:
		break;
	}
}

void SocketWriter::appendWriteBuffer(char* buf, int size)
{
	if (m_buf == nullptr)
	{
		m_buf = buf;
		m_size = size;
		m_remainSize = size;
		m_writePos = buf;
		appLogger()->trace("Assign ", size, " bytes to writting buffer of socket: ", m_sock);
		return;
	}
	char* newBuf = new char[m_remainSize + size]();
	memcpy(newBuf, m_writePos, m_remainSize);
	memcpy(newBuf + m_remainSize, buf, size);
	clear();
	m_buf = newBuf;
	m_size = m_remainSize + size;
	m_remainSize = m_remainSize + size;
	m_writePos = newBuf;
	appLogger()->trace("Append ", size, " bytes to writting buffer of socket: ", m_sock, ", Now remain size is: ", m_remainSize);
}

void SocketWriter::write()
{
	if (m_remainSize == 0)
	{
		appLogger()->warn("There is no bytes of socket ", m_sock, " writting buffer to send.");
		return;
	}
	m_state = WRITING;
	int sentSize = send(m_sock, m_writePos, m_remainSize, 0);
	m_writePos += sentSize;
	m_remainSize -= sentSize;
	appLogger()->trace("Sent ", sentSize, " bytes of writting buffer of socket: ", m_sock, ", Now remain size is: ", m_remainSize);
	if (m_remainSize == 0)
	{
		clear();
		m_state = READY;
	}
}

void Session::read()
{
	m_reader.read();
}

void Session::write()
{
	m_writer.write();
}

void Session::writeBuffer(char* buf, int bufSize)
{
	m_writer.appendWriteBuffer(buf, bufSize);
	if (m_writer.state() == SocketWriter::READY)
	{
		m_writer.write();
	}
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
				replyRegister(TEXT("2;用户名已经存在！"));
			}
			else
			{
				query->prepare("INSERT INTO player(name,password) VALUES(:name,:password)");
				query->bindValue(TEXT(":name"), playerName);
				query->bindValue(TEXT(":password"), password);
				query->exec();
				appLogger()->trace("Register succeeded! Socket: ", m_sock);
				replyRegister(TEXT("1;OK"));
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
				replyLogin(TEXT("2;用户名不能为空！"));
				return;
			}
			if (password.IsEmpty())
			{
				appLogger()->trace("Error: Player password for login is empty! Socket: ", m_sock);
				replyLogin(TEXT("2;密码不能为空！"));
				return;
			}
			auto query = Application::sharedInstance()->dbManager()->sqlite()->makeQuery();
			query->prepare("SELECT * FROM player WHERE name=:name");
			query->bindValue(TEXT(":name"), playerName);
			query->exec();
			if (!query->next())
			{
				appLogger()->trace("Error: Player named:", playerNameMb.c_str(), " not exists in DB! Socket: ", m_sock);
				replyLogin(TEXT("2;用户名或密码错误！"));
				return;
			}
			m_playerName = playerName;
			appLogger()->trace("Player named: ", playerNameMb.c_str(), " login succeeded! Associated socket is ", m_sock);
			replyLogin(TEXT("1;OK"));
		}
		break;
	case GET_PLAYER_LIST_REQUEST:
	    {
			appLogger()->trace("Handle socket ", m_sock, " get player list request. ");
		    auto allSession = Application::sharedInstance()->sessionMgr()->allSessions();
			CString body;
			for (auto iter = allSession.begin(); iter != allSession.end(); ++iter)
			{
				if (iter->second->playerName != m_playerName)
				{
					body += iter->second->playerName() + TEXT(";");
				}
			}
			appLogger()->trace("Reply player list result to socket ", m_sock, ". Body is ", body);
			std::string bodyUtf8 = StringUtil::CStringToUtf8(body);
			int bufSize = 3 + bodyUtf8.size();
			char* buf = new char[bufSize]();
			UINT8 eventId = GET_PLAYER_LIST_RESPONSE;
			UINT16 bodyLength = bodyUtf8.size();
			memcpy(buf, (char*)&eventId, 1);
			memcpy(buf + 1, (char*)&bodyLength, 2);
			memcpy(buf + 3, bodyUtf8.c_str(), bodyUtf8.size());
			writeBuffer(buf, bufSize);
	    }
		break;
	case CHALLENGE_FRIEND_REQUEST:
		{
			appLogger()->trace("Handle socket ", m_sock, " challenge friend request.");
			if (m_state == COMMUNICATING)
			{
				appLogger()->error("Can not challenge friend from socket: ", m_sock,
					". The socket is communicating with another friend.");
			}

			// 改变socket状态为正在通信状态
			m_state = COMMUNICATING;

			// 解析body内容
			std::string utf8Str(body.first, body.second);
			CString challengeInfo = StringUtil::Utf8ToCString(utf8Str);
			int seperatorPos = challengeInfo.Find(TEXT(';'));
			CString friendName = challengeInfo.Left(seperatorPos);
			CString gameName = challengeInfo.Mid(seperatorPos + 1);
			appLogger()->trace("Friend name is: ", StringUtil::CStringToMultiByte(friendName).c_str(),
				"Game name is :", StringUtil::CStringToMultiByte(gameName).c_str());

			appLogger()->trace("Sending challenge information to friend ", StringUtil::CStringToMultiByte(friendName).c_str());
			// 找到对方的session，发送消息给对方
			auto friendSession = Application::sharedInstance()->sessionMgr()->findSession(friendName);
			if (friendSession == nullptr)
			{
				m_state = AVAILABLE;
				appLogger()->error("Can not find friend named ", StringUtil::CStringToMultiByte(friendName).c_str());

				// 返回错误信息给发送者
				CString responseBody = TEXT("3;无法找到对方，对方可能已经下线");
				std::string bodyUtf8 = StringUtil::CStringToUtf8(responseBody);
				int bufSize = 3 + bodyUtf8.size();
				char* buf = new char[bufSize]();
				UINT8 eventId = CHALLENGE_FRIEND_RESPONSE;
				UINT16 bodyLength = bodyUtf8.size();
				memcpy(buf, (char*)&eventId, 1);
				memcpy(buf + 1, (char*)&bodyLength, 2);
				memcpy(buf + 3, bodyUtf8.c_str(), bodyUtf8.size());
				writeBuffer(buf, bufSize);

				break;
			}
			m_friendName = friendName;  // 设置朋友名称
			appLogger()->trace("Sending challenge data to friend soekt ", friendSession->socket());
			CString body;
			body = m_playerName + TEXT(";") + gameName;
			std::string bodyUtf8 = StringUtil::CStringToUtf8(body);
			int bufSize = 3 + bodyUtf8.size();
			char* buf = new char[bufSize]();
			UINT8 eventId = CHALLENGE_FRIEND_REQUEST;
			UINT16 bodyLength = bodyUtf8.size();
			memcpy(buf, (char*)&eventId, 1);
			memcpy(buf + 1, (char*)&bodyLength, 2);
			memcpy(buf + 3, bodyUtf8.c_str(), bodyUtf8.size());
			friendSession->writeBuffer(buf, bufSize);
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

void Session::replyRegister(const CString& body)
{
	appLogger()->trace("Reply register result to socket ", m_sock, ". Body is ", body);
	std::string bodyUtf8 = StringUtil::CStringToUtf8(body);
	int bufSize = 3 + bodyUtf8.size();
	char* buf = new char[bufSize]();
	UINT8 eventId = REG_RESULT;
	UINT16 bodyLength = bodyUtf8.size();
	memcpy(buf, &eventId, 1);
	memcpy(buf + 1, &bodyLength, 2);
	memcpy(buf + 3, bodyUtf8.c_str(), bodyUtf8.size());
	m_writer.appendWriteBuffer(buf, bufSize);
	if (m_writer.state() == SocketWriter::READY)
	{
		m_writer.write();
	}
}

void Session::replyLogin(const CString& body)
{
	appLogger()->trace("Reply login result to socket ", m_sock, ". Body is ", body);
	std::string bodyUtf8 = StringUtil::CStringToUtf8(body);
	int bufSize = 3 + bodyUtf8.size();
	char* buf = new char[bufSize]();
	UINT8 eventId = LOGIN_RESULT;
	UINT16 bodyLength = bodyUtf8.size();
	memcpy(buf, (char*)&eventId, 1);
	memcpy(buf + 1, (char*)&bodyLength, 2);
	memcpy(buf + 3, bodyUtf8.c_str(), bodyUtf8.size());
	m_writer.appendWriteBuffer(buf, bufSize);
	if (m_writer.state() == SocketWriter::READY)
	{
		m_writer.write();
	}
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

std::shared_ptr<Session> SessionMgr::findSession(const CString& playerName)
{
	std::string playerNameMb = StringUtil::CStringToMultiByte(playerName);
	appLogger()->trace("Finding session for player named: ", playerNameMb.c_str());
	for (auto pairSession : m_sessions)
	{
		if (pairSession.second->playerName() == playerName)
		{
			appLogger()->error("Session for player named: ", playerNameMb.c_str(), " has been found.");
			return pairSession.second;
		}
	}
	appLogger()->trace("Can not find session for player named: ", playerNameMb.c_str());
	return nullptr;
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
