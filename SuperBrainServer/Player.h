#pragma once

#include <atlstr.h>

class Player
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
	Player(SOCKET id, const CString& name) : m_id(id), m_name(name), m_state(UNAVAILABLE) {}

	SOCKET id() const { return m_id; }
	const CString& name() const { return m_name; }

	void setState(State state) { m_state = state; }
	State state() const { return m_state; }

	void setGame(const CString& gameId) { m_gameId = gameId; }
	const CString& gameId() const { return m_gameId; }

private:
	SOCKET m_id;
	CString m_name;
	State m_state;
	CString m_gameId;
};
