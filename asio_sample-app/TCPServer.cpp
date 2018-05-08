#include "TCPServer.h"


TCPServer::TCPServer(asio_service& core) :
	Myacceptor(core,
		asio_endpoint(asio_tcp_ip::v4(), PORT))
{
		Init(50);
}
