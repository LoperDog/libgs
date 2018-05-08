#include"common.h"
#include"TCPServer.h"

int main() {

	asio_service asio_core;
	TCPServer firstServer(asio_core);
	asio_core.run();


	return 0;
}