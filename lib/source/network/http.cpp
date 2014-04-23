#include "stdafx.h"
#include <lib/network/http.h>
#include <sstream>

namespace lib {
namespace network {

bool Http::s_initialized = false;

bool Http::initialize()
{
	WSADATA wsaData;
	if (!s_initialized) {
		if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
			DEBUG_PRINT("WSAStartup failed\n");
			return false;
		}
		s_initialized = true;
	}
	return true;
}

bool Http::destroy()
{
	if (s_initialized) {
		if (WSACleanup())  return false;
	}
	return true;
}

Http::Http()
: m_state(STATE_NONE)
, m_addInfo(NULL)
, m_socket(INVALID_SOCKET)
, m_method(METHOD_GET)
, m_noblockingmode(1)
, m_https(false)
, m_port(0)
{
}

Http::~Http()
{
	close();
}

void Http::close()
{
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
	m_state = STATE_NONE;
	freeaddrinfo(m_addInfo);
	m_addInfo = NULL;
}

void Http::get(const char* address)
{
	close();
	m_noblockingmode = 0;
	parseURL(&m_address, &m_path, &m_port, address);
	m_data = "";
	m_method = METHOD_GET;

	m_state = STATE_INIT;

}

void Http::update(float delta)
{
	switch (m_state) {
	case STATE_NONE:
		break;
	case STATE_INIT:
	{
		m_socket = socket(AF_INET, SOCK_STREAM, 0);

		if(m_socket == INVALID_SOCKET) {
			printf("Http socket error. %d", WSAGetLastError());
			m_state = STATE_RECIEVE_END;
			break;
		}
		if (ioctlsocket(m_socket, FIONBIO, &m_noblockingmode)) {
			DEBUG_PRINT("Http socket error %d\n", WSAGetLastError());
			m_state = STATE_RECIEVE_END;
			break;
		}
		
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_family = AF_INET;
		int err;
		if ((err = getaddrinfo(m_address.c_str(), NULL, &hints, &m_addInfo))) {
			DEBUG_PRINT("Http addinfo error %d\n", err);
			m_state = STATE_RECIEVE_END;
			break;
		}
	
		m_state = STATE_CONNECT;
		break;
	}
	case STATE_CONNECT:
	{
		if (!m_addInfo) {
			m_state = STATE_RECIEVE_END;
		} else {
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(m_port);
//			addr.sin_addr.S_un.S_addr = inet_addr(m_address.c_str());
			addr.sin_addr.S_un = ((struct sockaddr_in *)(m_addInfo->ai_addr))->sin_addr.S_un;
			DEBUG_PRINT("Http request address \"%s\"(%s)\n", m_address.c_str(), inet_ntoa(addr.sin_addr));
			if (connect(m_socket, (struct sockaddr *)&addr, sizeof(addr))) {
				DEBUG_PRINT("Http connect error %d\n", WSAGetLastError());
				m_state = STATE_RECIEVE_END;
			} else {
				m_state = STATE_SEND_REQUEST;
			}
		}
		break;
	}
	case STATE_SEND_REQUEST:
	{
		std::stringstream send_request;
		switch (m_method) {
		case METHOD_GET:
			send_request << "GET / HTTP/1.0\r\n";
			break;
		case METHOD_POST:
			send_request << "POST / HTTP/1.0\r\n";
			break;
		default:
			ASSERT_MES(false, "Http invalid method. method=%d\n", m_method);
		}
		send_request << "Host: " << m_address << "\r\n";
		send_request << "\r\n";
		DEBUG_PRINT("Http send request = \"%s\"\n", send_request.str().c_str());
		int n = send(m_socket, send_request.str().c_str(), send_request.str().size(), 0);
		if (n < 0) {
			DEBUG_PRINT("Http send error %d\n", WSAGetLastError());
			m_state = STATE_RECIEVE_END;
		} else {
			m_state = STATE_RECIEVE_WAIT;
		}
		break;
	}
	case STATE_RECIEVE_WAIT:
	{
		char buf[256];
		memset(buf, 0, sizeof(buf));
		int n = recv(m_socket, buf, sizeof(buf), 0);
		if (n < 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				DEBUG_PRINT("Http recieve error : 0x%x\n", WSAGetLastError());
				m_state = STATE_RECIEVE_END;
			}
		} else {
			m_data = buf;
			m_state = STATE_RECIEVE;
		}
		break;
	}
	case STATE_RECIEVE:
	{
		char buf[256];
		int n = recv(m_socket, buf, sizeof(buf)-1, 0);
		if (n == 0) {
			m_state = STATE_RECIEVE_END;
		} else if (n < 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				DEBUG_PRINT("Http recieve error : 0x%x\n", WSAGetLastError());
				m_state = STATE_RECIEVE_END;
			}
		} else {
			buf[n] = '\0';
//			DEBUG_PRINT(buf);
			m_data += buf;
		}
		break;
	}
	case STATE_RECIEVE_END:
	{
		close();
		m_state = STATE_FINISH;
		break;
	}
	case STATE_FINISH:
	{
		break;
	}
	default:
		ASSERT_MES(false, "Http state invalid. state=%d\n", m_state);
	}
}

void Http::parseURL(std::string* host, std::string* path, u_short* port, const char* address)
{
	*host = "";
	*path = "";
	*port = 80;

	// HTTP://削除
	const char* p = NULL;
	if (strstr(address, "http://") == address) {
		address += strlen("http://");
	} else if (strstr(address, "https://") == address) {
		address += strlen("https://");
		m_https = true;
	}
	// host取り出し
	p = strstr(address, "/");
	if (!p) {
		*host = address;
		*path = "/";
	} else {
		*host = "";
		while(p > address) {
			*host += *address;
			address++;
		}
		*path = address;
	}
	// port
	std::string::size_type pos = host->find(':', 0);
	if (pos != std::string::npos) {
		*port = atoi(host->substr(pos+1).c_str());
		host->erase(pos);
	}

	DEBUG_PRINT("%s %s %d\n", host->c_str(), path->c_str(), *port);
}



} // namespace network
} // namespace lib