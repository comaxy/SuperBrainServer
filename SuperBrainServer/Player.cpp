#include "Player.h"
#include "Application.h"
#include "LoggerDef.h"

void Player::setState(State state) 
{
	if (m_state != state)
	{
		m_state = state;
		LOG_TRACE("Player ", m_id, " change state to ", state);
	}
}
