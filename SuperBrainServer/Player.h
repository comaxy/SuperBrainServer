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
	Player() : m_name(), m_state(UNAVAILABLE) {}

	void setName(const CString& name) { m_name; }
	const CString& name() const { return m_name; }

	void setState(State state) { m_state = state; }
	State state() const { return m_state; }

private:
	CString m_name;
	State m_state;
};
