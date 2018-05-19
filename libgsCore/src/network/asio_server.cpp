#include "stdafx.h"
#include "network/asio_server.h"

void app::TCPServer::OnSend(boost::shared_ptr<USession> session_)
{
	std::cout << "Server : doing send" << std::endl;
	BOOST_ASSERT(!session_->getSendBufferList().empty());
	typedef boost::function<void(
		const  boost::system::error_code &, const size_t)> sendCb;

	sendCb on_send =
		bind(&TCPServer::SendHandling, shared_from_this(), _1);

	session_->Socket().async_write_some(
		boost::asio::buffer(session_->getSendBufferList().front()->c_str(),
			session_->getSendBufferList().front()->size()), 
		on_send);

	std::cout << "Server size : " << session_->getSendBufferList().front()->size() << std::endl;

	session_->getSendBufferList().pop();
}

void app::TCPServer::RecvHandling(
  boost::shared_ptr<USession> session_,
  const boost::system::error_code& error,
  const size_t si) {
  
  // Packet 확인 테스트용.
  memcpy(&RecvPacket, buffer_.data(), sizeof(Buffer));
  std::cout << "Client Server memcpy headersize : " << RecvPacket.header << std::endl;
  std::cout << "Client Server memcpy size : " << RecvPacket.size << std::endl;

  memcpy(&RecvData, buffer_.data(), sizeof(Buffer));
  std::cout << "Server memcpy body : " << RecvData.body.data() << std::endl;

  OnRecv(session_);
}

void app::TCPServer::SendHandling(const boost::system::error_code & error)
{
	if (error != boost::system::errc::success) {
		std::cout << "Send Error : " << error.message() << std::endl;
	}
}

void app::TCPServer::OnRecv(boost::shared_ptr<USession> session_)
{
	std::cout << "Server : Start recv" << std::endl;
	memset(&buffer_, '\0', sizeof(buffer_));

  session_->Socket().async_read_some(
    boost::asio::buffer(buffer_),
    boost::bind(&TCPServer::RecvHandling, this,
      session_,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred)
  );

}


//--------------------------------------------

void app::TCPClient::OnRecv()
{
	std::cout << "Client : Start recv" << std::endl;
	memset(&buffer_, '\0', sizeof(buffer_));

	socket_.async_read_some(
		boost::asio::buffer(buffer_),
		boost::bind(&TCPClient::RecvHandling, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void app::TCPClient::RecvHandling(const boost::system::error_code& error, const size_t si)
{
	std::cout << "Client doing recv!" << std::endl;
	std::cout << buffer_.data() << std::endl;
	std::cout << "Client size : " << std::strlen(buffer_.data()) << std::endl;
}

void app::TCPClient::testSend()
{
  TestData.header = 1;
  memcpy(&TestData.body, "TEST Send", 10);
  TestData.size = TestData.body.size();
  
  char* buf = new char[sizeof(Packet) + strlen(TestData.body.data())];

  std::cout << "sizeof Size : " << sizeof(buf) << std::endl;
  std::cout << "strlen Size : " << std::strlen(buf) << std::endl;

  memcpy(buf, &TestData, sizeof(sizeof(Packet) + strlen(TestData.body.data())));

  boost::asio::async_write(
    socket_,
    boost::asio::buffer(buf, TestData.size),
    [this](boost::system::error_code ec, std::size_t)
  {
    if (!ec) {

      std::cout << "client Send No err" << std::endl;
    }
    else {
      std::cout << "client Send Error" << std::endl;
    }
  });
}