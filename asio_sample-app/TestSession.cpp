#include "TestSession.h"

TestSession::TestSession(int index, asio_service& service) :MySocket(service) {

}
void TestSession::PostRecv() {

	memset(&recvbuff, '\0', sizeof(recvbuff));
	MySocket.async_read_some
	(
		boost::asio::buffer(recvbuff),
		boost::bind(&TestSession::Handle_Recv, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void TestSession::Handle_Recv(const boost_error_handle& err, size_t si) {
	if (err) {

		if (err == boost::asio::error::eof)
		{
			std::cout << "클라이언트와 연결이 끊어졌습니다" << std::endl;
		}
		else
		{
			std::cout << "error No: " << err.value() << " error Message: "
				<< err.message() << std::endl;
		}
	}
	else {
		const std::string strRecvMessage = recvbuff.data();
		std::cout << "클라이언트에서 받은 메시지: " << strRecvMessage
			<< ", 받은 크기: " << si << std::endl;
		char szMessage[128] = { 0, };
		sprintf_s(szMessage, 128 - 1, "Re:%s", strRecvMessage.c_str());
		sendMsg = szMessage;

		boost::asio::async_write(MySocket, boost::asio::buffer(sendMsg),
			boost::bind(&TestSession::Handle_Send, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
		);
		PostRecv();
	}
}