#ifndef TESTSESSION_H
#define TESTSESSION_H
#include"common.h"
class TCPServer;

class TestSession
{
public:
	TestSession(int index, asio_service& service);

	asio_tcp_socket& Socket() {
		return MySocket;
	}
	// ���ú� �غ� �Ѵ�.
	void PostRecv();
private :
	void Handle_Send (const boost_error_handle& err, size_t si) {

	}
	void Handle_Recv(const boost_error_handle& err, size_t si);

	asio_tcp_socket MySocket;
	// ���� ������
	std::array<char, 128> recvbuff;
	// �������̹�
	std::string sendMsg ;
};

#endif