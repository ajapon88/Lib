
#ifndef __LIB_NETWORK_HTTP_H__
#define __LIB_NETWORK_HTTP_H__
#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

namespace lib {
namespace network {
class Http
{
public:
	enum STATE {
		STATE_NONE,
		STATE_INIT,
		STATE_CONNECT,
		STATE_SEND_REQUEST,
		STATE_RECIEVE_WAIT,
		STATE_RECIEVE,
		STATE_RECIEVE_END,
		STATE_FINISH,
	};
	enum METHOD {
		METHOD_GET,
		METHOD_POST,
	};
public:
	static bool initialize();
	static bool destroy();

	static bool s_initialized;
public:
	Http();
	~Http();

	void get(const char *address);
	void post(const char *address);
	void update(float delta);
	void close();
	STATE getState() { return m_state; }
	const std::string& getData() { return m_data; }

	void parseURL(std::string* host, std::string* path, u_short* port, const char* address);

private:
	STATE m_state;
	struct addrinfo *m_addInfo;
	SOCKET m_socket;
	METHOD m_method;
	u_long m_noblockingmode;
	bool m_https;
	std::string m_address;
	std::string m_path;
	u_short m_port;
	std::string m_data;
};


} // namespace network
} // namespace lib

#endif // __LIB_NETWORK_HTTP_H__