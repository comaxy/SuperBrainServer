#include "Session.h"
#include "StringUtil.h"
#include "SockEventDef.h"
#include "LoggerDef.h"
#include "DbManager.h"
#include "PlayerManager.h"
#include "SessionManager.h"
#include "GameManager.h"
#include "Player.h"
#include "Game.h"

Session::Session(SOCKET sock)
	: m_socket(new Socket(sock))
{
}

void Session::readDone(UINT8 eventId, const std::pair<char*, UINT16>& body)
{
	switch (eventId)
	{
	case REGISTER:
		handleRegister(body);
		break;
	case LOGIN:
		handleLogin(body);
		break;
	case GET_PLAYER_LIST_REQUEST:
		handleGetPlayerList(body);
		break;
	case CHALLENGE_FRIEND_REQUEST:
		handleChallengeFriendRequest(body);
		break;
	case CHALLENGE_FRIEND_RESPONSE:
		handleChallengeFriendResponse(body);
		break;
	default:
		handleEventDefault(eventId, body);
		break;
	}
}

bool Session::initialize()
{
	m_socket->reader()->setDelegate(this);
	return true;
}

void Session::handleRegister(const std::pair<char*, UINT16>& body)
{
	std::string utf8Str(body.first, body.second);
	CString playerInfo = StringUtil::Utf8ToCString(utf8Str);
	int seperatorPos = playerInfo.Find(TEXT(';'));
	CString playerName = playerInfo.Left(seperatorPos);
	CString password = playerInfo.Mid(seperatorPos + 1);
	std::string playerNameMb = StringUtil::CStringToMultiByte(playerName);
	LOG_TRACE("Handle socket ", m_socket->socket(), " register event. ",
		"Player name: ", playerNameMb.c_str());
	if (playerName.IsEmpty())
	{
		LOG_TRACE("Error: Player name for register is empty! Socket: ", m_socket->socket());
		return;
	}
	if (password.IsEmpty())
	{
		LOG_TRACE("Error: Player password for register is empty! Socket: ", m_socket->socket());
		return;
	}

	auto query = Application::sharedInstance()->dbManager()->sqlite()->makeQuery();
	query->prepare("SELECT * FROM player WHERE name=:name");
	query->bindValue(TEXT(":name"), playerName);
	query->exec();
	if (query->next())
	{
		LOG_TRACE("Register failed! Player named ", playerNameMb.c_str(), " has already exists. Socket: ", m_socket->socket());
		replyRegister(TEXT("2;用户名已经存在！"));
	}
	else
	{
		query->prepare("INSERT INTO player(name,password) VALUES(:name,:password)");
		query->bindValue(TEXT(":name"), playerName);
		query->bindValue(TEXT(":password"), password);
		query->exec();
		LOG_TRACE("Register succeeded! Socket: ", m_socket->socket());
		replyRegister(TEXT("1;OK"));

		auto player = Application::sharedInstance()->playerManager()->newPlayer(m_socket->socket(), playerName);
		player->setState(Player::AVAILABLE);
	}
}

void Session::replyRegister(const CString& body)
{
	LOG_TRACE("Reply register result to socket ", m_socket->socket(), ". Body is ", body);
	sendStringBody(REG_RESULT, body);
}

void Session::handleLogin(const std::pair<char*, UINT16>& body)
{
	std::string utf8Str(body.first, body.second);
	CString playerInfo = StringUtil::Utf8ToCString(utf8Str);
	int seperatorPos = playerInfo.Find(TEXT(';'));
	CString playerName = playerInfo.Left(seperatorPos);
	CString password = playerInfo.Mid(seperatorPos + 1);
	std::string playerNameMb = StringUtil::CStringToMultiByte(playerName);
	LOG_TRACE("Handle socket ", m_socket->socket(), " login event. ",
		"Player name: ", playerNameMb.c_str());
	if (playerName.IsEmpty())
	{
		LOG_TRACE("Error: Player name for login is empty! Socket: ", m_socket->socket());
		replyLogin(TEXT("2;用户名不能为空！"));
		return;
	}
	if (password.IsEmpty())
	{
		LOG_TRACE("Error: Player password for login is empty! Socket: ", m_socket->socket());
		replyLogin(TEXT("2;密码不能为空！"));
		return;
	}
	auto query = Application::sharedInstance()->dbManager()->sqlite()->makeQuery();
	query->prepare("SELECT * FROM player WHERE name=:name");
	query->bindValue(TEXT(":name"), playerName);
	query->exec();
	if (!query->next())
	{
		LOG_TRACE("Error: Player named:", playerNameMb.c_str(), " not exists in DB! Socket: ", m_socket->socket());
		replyLogin(TEXT("2;用户名或密码错误！"));
		return;
	}
	auto player = Application::sharedInstance()->playerManager()->newPlayer(m_socket->socket(), playerName);
	player->setState(Player::AVAILABLE);
	LOG_TRACE("Player named: ", playerNameMb.c_str(), " login succeeded! Associated socket is ", m_socket->socket());
	replyLogin(TEXT("1;OK"));
}

void Session::replyLogin(const CString& body)
{
	LOG_TRACE("Reply login result to socket ", m_socket->socket(), ". Body is ", body);
	sendStringBody(LOGIN_RESULT, body);
}

void Session::handleGetPlayerList(const std::pair<char*, UINT16>& body)
{
	LOG_TRACE("Handle socket ", m_socket->socket(), " get player list request. ");
	auto allSession = Application::sharedInstance()->sessionManager()->allSessions();
	CString bodyReply;
	auto self = Application::sharedInstance()->playerManager()->findPlayer(m_socket->socket());
	for (auto iter = allSession.begin(); iter != allSession.end(); ++iter)
	{
		auto player = Application::sharedInstance()->playerManager()->findPlayer(iter->second->socket()->socket());
		if (player->name() != self->name())
		{
			bodyReply += player->name() + TEXT(";");
		}
	}
	LOG_TRACE("Reply player list result to socket ", m_socket->socket());
	sendStringBody(GET_PLAYER_LIST_RESPONSE, bodyReply);
}

void Session::sendStringBody(UINT8 eventId, const CString& responseBody)
{
	std::string bodyUtf8 = StringUtil::CStringToUtf8(responseBody);
	sendStringBody(eventId, bodyUtf8);
}

void Session::sendStringBody(UINT8 eventId, const std::string& bodyUtf8)
{
	int bufSize = 3 + bodyUtf8.size();
	char* buf = new char[bufSize]();
	UINT16 bodyLength = bodyUtf8.size();
	memcpy(buf, (char*)&eventId, 1);
	memcpy(buf + 1, (char*)&bodyLength, 2);
	memcpy(buf + 3, bodyUtf8.c_str(), bodyUtf8.size());
	m_socket->writeBuffer(buf, bufSize);
}

void Session::handleChallengeFriendRequest(const std::pair<char*, UINT16>& body)
{
	LOG_TRACE("Handle socket ", m_socket->socket(), " challenge friend request.");
	auto player = Application::sharedInstance()->playerManager()->findPlayer(m_socket->socket());
	if (player->state() == Player::COMMUNICATING)
	{
		LOG_ERROR("Can not challenge friend from socket: ", m_socket->socket(),
			". The socket is communicating with another friend.");
		return;
	}

	// 改变socket状态为正在通信状态
	player->setState(Player::COMMUNICATING);

	// 解析body内容
	std::string utf8Str(body.first, body.second);
	CString challengeInfo = StringUtil::Utf8ToCString(utf8Str);
	int seperatorPos = challengeInfo.Find(TEXT(';'));
	CString friendName = challengeInfo.Left(seperatorPos);
	CString gameName = challengeInfo.Mid(seperatorPos + 1);
	LOG_TRACE("Friend name is: ", StringUtil::CStringToMultiByte(friendName).c_str(),
		"Game name is :", StringUtil::CStringToMultiByte(gameName).c_str());

	LOG_TRACE("Sending challenge information to friend ", StringUtil::CStringToMultiByte(friendName).c_str());
	// 找到对方的session，发送消息给对方
	auto friendPlayer = Application::sharedInstance()->playerManager()->findPlayer(friendName);
	if (friendPlayer == nullptr)
	{
		LOG_ERROR("Can not find friend named: ", StringUtil::CStringToMultiByte(friendName).c_str());

		// 返回错误信息给发送者
		sendStringBody(CHALLENGE_FRIEND_RESPONSE, TEXT("3;无法找到对方，对方可能已经下线"));
		return;
	}
	auto friendSession = Application::sharedInstance()->sessionManager()->findSession(friendPlayer->id());
	if (friendSession == nullptr)
	{
		player->setState(Player::AVAILABLE);
		LOG_ERROR("Can not find session of friend named ", StringUtil::CStringToMultiByte(friendName).c_str());

		// 返回错误信息给发送者
		sendStringBody(CHALLENGE_FRIEND_RESPONSE, TEXT("3;无法找到对方，对方可能已经下线"));
		return;
	}
	
	friendPlayer->setState(Player::COMMUNICATING);

	auto game = Application::sharedInstance()->gameManager()->newGame(gameName);
	game->addPlayer(player->id());
	game->addPlayer(friendPlayer->id());
	player->setGame(game->id());
	friendPlayer->setGame(game->id());

	LOG_TRACE("Sending challenge data to friend soekt ", friendSession->socket());
	sendStringBody(CHALLENGE_FRIEND_REQUEST, player->name() + TEXT(";") + gameName);
}

void Session::handleChallengeFriendResponse(const std::pair<char*, UINT16>& body)
{
	auto player = Application::sharedInstance()->playerManager()->findPlayer(m_socket->socket());
	auto game = Application::sharedInstance()->gameManager()->findGame(player->gameId());

	LOG_TRACE("Handle socket ", m_socket->socket(), " challenge friend response.");
	if (player->state() != Player::COMMUNICATING)
	{
		LOG_ERROR("Can not reply challenge friend from socket: ", m_socket->socket(),
			". The socket is not in communicating state.");
		return;
	}

	auto friendPlayerId = game->findFriendPlayerId(m_socket->socket());
	auto friendPlayer = Application::sharedInstance()->playerManager()->findPlayer(friendPlayerId);
	
	LOG_TRACE("Sending challenge reply information to friend ", StringUtil::CStringToMultiByte(friendPlayer->name()).c_str());
	// 找到对方的session，发送消息给对方
	auto friendSession = Application::sharedInstance()->sessionManager()->findSession(friendPlayerId);
	if (friendSession == nullptr)
	{
		player->setState(Player::AVAILABLE);
		LOG_ERROR(__FUNCDNAME__, " Can not find friend named ", StringUtil::CStringToMultiByte(friendPlayer->name()).c_str());
		return;
	}

	LOG_TRACE("Sending challenge reply data to friend soekt ", friendSession->socket());
	std::string bodyUtf8(body.first, body.second);
	sendStringBody(CHALLENGE_FRIEND_RESPONSE, bodyUtf8);

	// 如果同意，则开始游戏
	CString strBody = StringUtil::Utf8ToCString(bodyUtf8);
	if (strBody.Left(1) == TEXT("1"))
	{
		game->start();
	}
}

void Session::handleEventDefault(UINT8 eventId, const std::pair<char*, UINT16>& body)
{
	auto player = Application::sharedInstance()->playerManager()->findPlayer(m_socket->socket());
	auto game = Application::sharedInstance()->gameManager()->findGame(player->gameId());
	game->handleEvent(m_socket->socket(), eventId, body);
}
