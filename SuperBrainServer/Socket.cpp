#include "Socket.h"
#include "LoggerDef.h"

void SocketReader::read()
{
	switch (m_state)
	{
	case READING_HEADER:
		{
			LOG_TRACE("Socket ", m_sock, " reading header.");
			int recvSize = recv(m_sock, m_readPos, m_remainSize, 0);
			m_remainSize -= recvSize;
			if (m_remainSize == 0)
			{
				LOG_TRACE("Socket ", m_sock, " read header done.");

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
					LOG_TRACE("Socket ", m_sock, " read body done.");
					LOG_TRACE("=============== EVENT RECEIVED DONE ===============");
					LOG_TRACE("Event Id: ", *(UINT8*)&m_header[0], ", Body Length: ", m_bodySize);
					LOG_TRACE("===================================================");
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
			break;
		}
	case READING_BODY:
		{
			LOG_TRACE("Socket ", m_sock, " reading body.");
			int recvSize = recv(m_sock, m_readPos, m_remainSize, 0);
			m_remainSize -= recvSize;
			if (m_remainSize == 0)
			{
				LOG_TRACE("Socket ", m_sock, " read body done.");
				LOG_TRACE("=============== EVENT RECEIVED DONE ===============");
				LOG_TRACE("Event Id: ", *(UINT8*)&m_header[0], ", Body Length: ", m_bodySize);
				LOG_TRACE("===================================================");
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
			break;
		}
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
		LOG_TRACE("Assign ", size, " bytes to writting buffer of socket: ", m_sock);
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
	LOG_TRACE("Append ", size, " bytes to writting buffer of socket: ", m_sock, ", Now remain size is: ", m_remainSize);
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
	LOG_TRACE("Sent ", sentSize, " bytes of writting buffer of socket: ", m_sock, ", Now remain size is: ", m_remainSize);
	if (m_remainSize == 0)
	{
		clear();
		m_state = READY;
	}
}


Socket::Socket(SOCKET socket)
	: m_socket(socket)
	, m_reader(std::make_shared<SocketReader>(socket))
	, m_writer(std::make_shared<SocketWriter>(socket))
{
}

void Socket::read()
{
	m_reader->read();
}

void Socket::write()
{
	m_writer->write();
}

void Socket::writeBuffer(char* buf, int bufSize)
{
	m_writer->appendWriteBuffer(buf, bufSize);
	if (m_writer->state() == SocketWriter::READY)
	{
		m_writer->write();
	}
}
