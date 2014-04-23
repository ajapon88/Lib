
#ifndef __LIB_NETWORK_HTTP_H__
#define __LIB_NETWORK_HTTP_H__
#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>

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
		STATE_ERROR,
	};
	enum METHOD {
		METHOD_GET,
		METHOD_POST,
	};
	typedef std::vector<unsigned char> RecvData;
	typedef std::pair<std::string, std::string> HeaerFieldElement;
	typedef std::vector<HeaerFieldElement> HeaderFieldList;
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
	const char* getHeader() { return m_header.c_str(); }
	const unsigned char* getData() { return m_data.data(); }
	unsigned int getSize() { return m_data.size(); }

	void parseURL(std::string* host, std::string* path, u_short* port, const char* address);
	void parseHeader();
	int getStatusCode() { return m_statusCode; }
	const char* getHeaderField(const char* field);

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
	RecvData m_data;
	std::string m_header;
	HeaderFieldList m_headerFields;
	bool m_isRecvData;
	int m_statusCode;
};


} // namespace network
} // namespace lib

#endif // __LIB_NETWORK_HTTP_H__