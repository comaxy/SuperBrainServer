#include "Socket.h"

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
