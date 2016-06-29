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
		READ_DONE,
	};

public:
	SocketReader(SOCKET sock) 
		: m_sock(sock), m_state(READING_HEADER)
		, m_header(), m_body(0), m_bodySize(0) 
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

class Session : public SocketReaderDelegate
{
public:
	enum State
	{
		READY,
		READING,
		WRITING,
	};

public:
	Session(SOCKET sock) : m_sock(sock), m_state(READY), m_reader(sock) {}
	void setState(State state) { m_state = state; }
	void read();

	virtual void readDone(UINT8 eventId, const std::pair<char*, UINT16>& body) override;

private:
	SOCKET m_sock;
	State m_state;
	SocketReader m_reader;
};

class SessionMgr
{
public:
	std::shared_ptr<Session> newSession(SOCKET sock);
	std::shared_ptr<Session> findSession(SOCKET sock);

private:
	std::map<SOCKET, std::shared_ptr<Session>> m_sessions;
};
