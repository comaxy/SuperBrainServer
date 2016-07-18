#pragma once

#include "Socket.h"
#include <atlstr.h>
#include <memory>
#include <string>

class Session : public SocketReaderDelegate
{
public:
	Session(SOCKET sock);
	bool initialize();
	std::shared_ptr<Socket> socket() { return m_socket; }
	virtual void readDone(UINT8 eventId, const std::pair<char*, UINT16>& body) override;

private:
	void handleRegister(const std::pair<char*, UINT16>& body);
	void replyRegister(const CString& body);
	void handleLogin(const std::pair<char*, UINT16>& body);
	void replyLogin(const CString& body);
	void handleGetPlayerList(const std::pair<char*, UINT16>& body);
	void handleChallengeFriendRequest(const std::pair<char*, UINT16>& body);
	void handleChallengeFriendResponse(const std::pair<char*, UINT16>& body);
	void handleEventDefault(UINT8 eventId, const std::pair<char*, UINT16>& body);
	void sendStringBody(UINT8 eventId, const CString& body);
	void sendStringBody(UINT8 eventId, const std::string& bodyUtf8);

private:
	std::shared_ptr<Socket> m_socket;
};
