#pragma once

#include <winsock.h>
#include <memory>

class SocketReaderDelegate
{
public:
	virtual void readDone(UINT8 eventId, const std::pair<char*, UINT16>& body) = 0;
};

class SocketReader
{
	enum State
	{
		READING_HEADER,
		READING_BODY,
	};

public:
	SocketReader(SOCKET sock)
		: m_sock(sock), m_state(READING_HEADER)
		, m_body(0), m_bodySize(0)
		, m_readPos(nullptr)
		, m_remainSize(0)
		, m_delegate(nullptr)
	{
		clear();
	}

	void clear()
	{
		m_state = READING_HEADER;
		if (m_body)
		{
			delete[] m_body; m_body = nullptr;
		}
		m_bodySize = 0;
		memset(m_header, 0, sizeof(m_header));
		m_readPos = m_header;
		m_remainSize = 3;
	}

	void read();

	void setDelegate(SocketReaderDelegate *delegate) { m_delegate = delegate; }

private:
	SOCKET m_sock;
	State m_state;
	char m_header[3];
	char* m_body;
	UINT16 m_bodySize;
	char* m_readPos;
	int m_remainSize;
	SocketReaderDelegate* m_delegate;
};

class SocketWriter
{
public:
	enum State
	{
		WRITING,
		READY,
	};

public:
	SocketWriter(SOCKET sock)
		: m_sock(sock), m_buf(nullptr), m_size(0), m_writePos(nullptr), m_remainSize(0), m_state(READY)
	{
	}

	~SocketWriter()
	{
		clear();
	}

	void clear()
	{
		if (m_buf)
		{
			delete[] m_buf;
			m_buf = nullptr;
		}
		m_size = 0;
		m_remainSize = 0;
		m_writePos = nullptr;
	}

	void appendWriteBuffer(char* buf, int size);
	void write();
	State state() const { return m_state; }

private:
	SOCKET m_sock;
	char* m_buf;
	int m_size;
	char* m_writePos;
	int m_remainSize;
	State m_state;
};

class Socket
{
public:
	Socket(SOCKET socket);

	SOCKET socket() const { return m_socket; }

	void read();
	void write();
	void writeBuffer(char* buf, int bufSize);

	std::shared_ptr<SocketReader> reader() { return m_reader; }
	std::shared_ptr<SocketWriter> writer() { return m_writer; }

private:
	SOCKET m_socket;
	std::shared_ptr<SocketReader> m_reader;
	std::shared_ptr<SocketWriter> m_writer;
};
