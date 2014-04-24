#include "stdafx.h"
#include <lib/network/http.h>
#include <lib/utility/utility.h>
#include <sstream>

#define RECV_SIZE 65535
#define RECV_BUFFER_SIZE (RECV_SIZE+1)

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
, m_isRecvData(false)
, m_statusCode(0)
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
	m_header.clear();
	m_data.clear();
	m_method = METHOD_GET;
	m_isRecvData = false;
	m_headerFields.clear();

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
			DEBUG_PRINT("[HTTP] socket error. %d", WSAGetLastError());
			m_state = STATE_ERROR;
			break;
		}
		if (ioctlsocket(m_socket, FIONBIO, &m_noblockingmode)) {
			DEBUG_PRINT("[HTTP] socket error %d\n", WSAGetLastError());
			m_state = STATE_ERROR;
			break;
		}
		
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_family = AF_INET;
		int err;
		if ((err = getaddrinfo(m_address.c_str(), NULL, &hints, &m_addInfo))) {
			DEBUG_PRINT("[HTTP] addinfo error %d\n", err);
			m_state = STATE_ERROR;
			break;
		}
	
		m_state = STATE_CONNECT;
		break;
	}
	case STATE_CONNECT:
	{
		if (!m_addInfo) {
			m_state = STATE_ERROR;
		} else {
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(m_port);
//			addr.sin_addr.S_un.S_addr = inet_addr(m_address.c_str());
			addr.sin_addr.S_un = ((struct sockaddr_in *)(m_addInfo->ai_addr))->sin_addr.S_un;
			DEBUG_PRINT("[HTTP] request address \"%s\"(%s)\n", m_address.c_str(), inet_ntoa(addr.sin_addr));
			if (connect(m_socket, (struct sockaddr *)&addr, sizeof(addr))) {
				DEBUG_PRINT("[HTTP] connect error %d\n", WSAGetLastError());
				m_state = STATE_ERROR;
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
			send_request << "GET " << m_path << " HTTP/1.0\r\n";
			break;
		case METHOD_POST:
			send_request << "POST " << m_path << " HTTP/1.0\r\n";
			break;
		default:
			ASSERT_MES(false, "[HTTP] invalid method. method=%d\n", m_method);
		}
		send_request << "Host: " << m_address << "\r\n";
		if (!m_basicAuth.empty()) {
			send_request << "Authorization: Basic " << m_basicAuth << "\r\n";
		}
		send_request << "\r\n";
		DEBUG_PRINT("[HTTP] send request = \"%s\"\n", send_request.str().c_str());
		int n = send(m_socket, send_request.str().c_str(), send_request.str().size(), 0);
		if (n < 0) {
			DEBUG_PRINT("[HTTP] send error %d\n", WSAGetLastError());
			m_state = STATE_ERROR;
		} else {
			m_state = STATE_RECIEVE_WAIT;
		}
		break;
	}
	case STATE_RECIEVE_WAIT:
	{
		char buf[RECV_BUFFER_SIZE];
		memset(buf, 0, sizeof(buf));
		int n = recv(m_socket, buf, sizeof(buf)-1, 0);
		if (n < 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				DEBUG_PRINT("[HTTP] recieve error : 0x%x\n", WSAGetLastError());
				m_state = STATE_ERROR;
			}
		} else {
			DEBUG_PRINT("[HTTP] recieve start.\n");
			buf[n] = '\0';
			// ヘッダのみ切り離す
			char* p = strstr(buf, "\r\n\r\n");
			if (p) {
				// ヘッダの終端が含まれていた
				p += strlen("\r\n");	// 改行が最後に入るように
				*p = '\0';
				m_header = buf;
				// リクエストボディ
				p += strlen("\r\n");
				int bodysize = n-(p-buf);
				if (bodysize > 0) {
					m_data.resize(bodysize);
					memcpy(&m_data[0], p, bodysize);
				}
				m_isRecvData = true;
				DEBUG_PRINT("[HTTP] header end\n");
			} else {
				// ヘッダ終わっていない
				m_header = buf;
				m_isRecvData = false;
			}
			m_state = STATE_RECIEVE;
		}
		break;
	}
	case STATE_RECIEVE:
	{
		char buf[RECV_BUFFER_SIZE];
		memset(buf, 0, sizeof(buf));
		int n = recv(m_socket, buf, sizeof(buf)-1, 0);
		if (n == 0) {
			m_state = STATE_RECIEVE_END;
		} else if (n < 0) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				DEBUG_PRINT("[HTTP] recieve error : 0x%x\n", WSAGetLastError());
				m_state = STATE_ERROR;
			}
		} else {
			buf[n] = '\0';
//			DEBUG_PRINT(buf);
			if (m_isRecvData) {
				RecvData::size_type size = m_data.size();
				m_data.resize(size+n);
				memcpy(&m_data[size], buf, n);
			} else {
				char* p = strstr(buf, "\r\n\r\n");
				if (p) {
					// ヘッダの終端が含まれていた
					p += strlen("\r\n");	// 改行が最後に入るように
					*p = '\0';
					m_header += buf;
					// リクエストボディ
					p += strlen("\r\n");
					int bodysize = n-(p-buf);
					m_data.resize(bodysize);
					memcpy(&m_data[0], p, bodysize);
					m_isRecvData = true;
					DEBUG_PRINT("[HTTP] header end\n");
				} else {
					// 先頭が改行コードなら追加
					p = buf;
					while(*p == '\r' || *p == '\n') {
						m_header += *p;
						p++;
					}
					std::string::size_type pos = m_header.find("\r\n\r\n");
					if (pos == std::string::npos) {
						// ヘッダ終わっていない
						m_header += p;
					} else {
						// 連結した結果ヘッダ終わってた
						m_header.erase(pos+strlen("\r\n"));	// 改行が最後に入るように
						int bodysize = n-(p-buf);
						if (bodysize > 0) {
							m_data.resize(bodysize);
							memcpy(&m_data[0], p, bodysize);
						}
						m_isRecvData = true;
						DEBUG_PRINT("[HTTP] header end\n");
					}
				}
			}
		}
		break;
	}
	case STATE_RECIEVE_END:
	{
		close();
		parseHeader();
		const char* content_type = getHeaderField("Content-Type");
		// Content-Type が text/*** なら'\0'追加
		if (content_type) {
			if (strncmp(content_type, "text/", strlen("text/")) == 0) {
				m_data.push_back('\0');
			}
		}
		m_state = STATE_FINISH;
		DEBUG_PRINT("[HTTP] recieve end.\n");
		break;
	}
	case STATE_FINISH:
	{
		break;
	}
	case STATE_ERROR:
	{
		close();
		m_state = STATE_FINISH;
		m_header.clear();
		m_data.clear();
		DEBUG_PRINT("[HTTP] error end.\n");
		break;
	}
	default:
		ASSERT_MES(false, "[HTTP] state invalid. state=%d\n", m_state);
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

void Http::parseHeader()
{
	std::string::size_type pos0, pos1;
	const char *separate_char = "\r\n";
	int separete_len = strlen(separate_char);

	// ステータスコード取得
	{
		m_statusCode = 0;
		pos1 = m_header.find(separate_char, 0);
		if (pos1 == std::string::npos) {
			DEBUG_PRINT("[HTTP] invalid header. \"%s\"\n", m_header.c_str());
			return;
		}
		std::string status_field = utility::trim(m_header.substr(0, pos1));
		std::string::size_type p0, p1;
		p0 = status_field.find(" ");
		p0 = status_field.find_first_not_of(" ", p0+1);
		p1 = status_field.find(" ", p0+1);
		if (p0 == std::string::npos || p1 == std::string::npos) {
			DEBUG_PRINT("[HTTP] invalid status field. \"%s\"\n", status_field.c_str());
			return;
		}
		std::string status_code = status_field.substr(p0, p1-p0);
		m_statusCode = atoi(status_code.c_str());
	}

	// フィールドを取得
	pos0 = pos1+separete_len;
	while((pos1 = m_header.find(separate_char, pos0)) != std::string::npos) {
		std::string field = m_header.substr(pos0, pos1-pos0);
		std::string::size_type parse_pos = field.find(':');
		if (parse_pos != std::string::npos) {
			std::string key = utility::trim(field.substr(0, parse_pos));
			std::string value = utility::trim(field.substr(parse_pos+1));
			m_headerFields.push_back(HeaerFieldElement::pair(key, value));
		} else {
			DEBUG_PRINT("[HTTP] invalid field. \"%s\"\n", field.c_str());
		}
		pos0 = pos1 + separete_len;
	}
}

const char* Http::getHeaderField(const char* field)
{
	for (HeaderFieldList::iterator it = m_headerFields.begin(); it != m_headerFields.end(); ++it) {
		// 大文字/小文字をみない
		if (stricmp(it->first.c_str(), field) == 0) {
			return it->second.c_str();
		}
	}
	return NULL;
}

void Http::setBasicAuthorization(const char* user, const char* passwd)
{
	if (!user || !passwd) {
		m_basicAuth.clear();
		return;
	}

	std::string base64;
	base64 = user;
	base64 += ':';
	base64 += passwd;
	
	m_basicAuth = utility::encodeBase64(base64.c_str());
}


} // namespace network
} // namespace lib