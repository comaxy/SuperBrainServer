#include "SessionMgr.h"
#include "StringUtil.h"
#include <atlstr.h>

void SocketReader::read()
{
	switch (m_state)
	{
	case READING_HEADER:
		{
			int recvSize = recv(m_sock, m_readPos, m_remainSize, 0);
			m_remainSize -= recvSize;
			if (m_remainSize == 0)
			{
				m_state = READING_BODY;
				m_bodySize = *(reinterpret_cast<UINT16*>(&m_header[1]));
				m_body = new char[m_bodySize]();
				m_readPos = m_body;
				m_remainSize = m_bodySize;
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
			int recvSize = recv(m_sock, m_readPos, m_remainSize, 0);
			m_remainSize -= recvSize;
			if (m_remainSize == 0)
			{
				m_state = READ_DONE;
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
	case 1:
		{
			
		}
		break;
	case 2:
		{
			std::string utf8Str(body.first, body.second);
			CString userInfo = StringUtil::Utf8ToCString(utf8Str);
			OutputDebugString(userInfo);
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
	m_sessions[sock] = std::make_shared<Session>(sock);
	m_sessions[sock]->initialize();
	return m_sessions[sock];
}

std::shared_ptr<Session> SessionMgr::findSession(SOCKET sock)
{
	return m_sessions[sock];
}
