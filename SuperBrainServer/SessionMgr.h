#pragma once

#include <atlstr.h>
#include <winsock.h>
#include <memory>
#include <map>

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
		memset(m_header, 0, sizeof (m_header));
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

class Session : public SocketReaderDelegate
{
public:
	enum State
	{
		UNAVAILABLE,
		AVAILABLE,
		COMMUNICATING,
		FIGHTING,
	};

public:
	Session(SOCKET sock) : m_sock(sock), m_reader(sock), 
		m_writer(sock), m_playerName(), m_state(UNAVAILABLE) {}
	bool initialize();
	void read();
	void write();
	void writeBuffer(char* buf, int bufSize);
	SOCKET socket() const { return m_sock; }

	virtual void readDone(UINT8 eventId, const std::pair<char*, UINT16>& body) override;

	CString playerName() const { return m_playerName; }
	void setFriendName(const CString& friendName) { m_friendName = friendName; }

private:
	void replyRegister(const CString& body);
	void replyLogin(const CString& body);

private:
	SOCKET m_sock;
	SocketReader m_reader;
	SocketWriter m_writer;
	CString m_playerName;
	State m_state;
	CString m_friendName;  // 正在与之交互的玩家名称，仅在交互和对战时保存对方玩家名称
};

class SessionMgr
{
public:
	std::shared_ptr<Session> newSession(SOCKET sock);
	std::shared_ptr<Session> findSession(SOCKET sock);
	std::shared_ptr<Session> findSession(const CString& playerName);
	void destorySession(SOCKET sock);
	std::map<SOCKET, std::shared_ptr<Session>> allSessions() const { return m_sessions; }

private:
	std::map<SOCKET, std::shared_ptr<Session>> m_sessions;
};
