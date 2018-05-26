#include "stdafx.h"

#include "network/asio_server.h"

namespace app {
  namespace {
    boost::asio::io_service ioservice(8);
  }

  void InitializeAsio() {
    const size_t result = ioservice.run();

    BOOST_ASSERT(result > 0);
  }

  TCPSocket CreateTCPSocket() { return TCPSocket(ioservice); }
}
// NOTE[loperdog] : ?„ì‹œë¡??œë²„ê°€ ?¬ë?ë¡??ì„±?˜ëŠ”ì§€ë¥??•ì¸?œë‹¤.
void app::TCPServer::Start() {
  TCPEndPoint ep(boost::asio::ip::tcp::v4(), property_.port);
  acceptor_ = boost::make_shared<TCPAsyncAcceptor>(
    ioservice,ep,this);

  BOOST_ASSERT(acceptor_);

  AsyncAccept();

}

// NOTE[loperdog] : ?´ë¼?´ì–¸?¸ë? ë§Œë“¤???€?¥í•œ??

void app::TCPServer::AsyncAccept() {
  boost::shared_ptr<TCPClient> client =
    boost::make_shared<TCPClient>();
  BOOST_ASSERT(client);
  

  acceptor_->async_accept(
    client->Socket(),
    boost::bind(&TCPServer::OnAccepted, this,
      client,boost::asio::placeholders::error
    ));
}

void app::TCPServer::OnAccepted(const boost::shared_ptr<TCPClient> &client,
  const boost::system::error_code &err) {

  AsyncAccept();

  // TODO[loperdog] : clientë¥?ì¶”ê??œë‹¤.
  // ?´ë‹¨ê³„ì—??sharedë¡?ë²„í¼ë¥?ë§Œë“¤??ì¤˜ë„ ì¢‹ì?ë§?ê´€ë¦¬í•˜??ë²„í¼?€??ë§Œë“¤ê³??¶ë‹¤.
  boost::shared_ptr<libgs::Uuid> newuuid = boost::make_shared<libgs::Uuid>();
  BOOST_ASSERT(newuuid);
  
  userlist.insert(std::make_pair(newuuid, client));

  //NOTE[loperdog] : client set Recv
  client->RecvHandling();
}
/*
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
  
  // Packet ?•ì¸ ?ŒìŠ¤?¸ìš©.
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
*/
// NOTE[loperdog] : tcp cilentë¥?tcp server?´ë¡œ ê·€???˜ë„ë¡??¤ì‹œ ë§Œë“ ??
app::TCPClient::TCPClient() : socket_(CreateTCPSocket()) {}

void app::TCPClient::OnSend(const boost::system::error_code & error) {

}

void app::TCPClient::SendHandling() {

}

void app::TCPClient::OnRecv(const boost::system::error_code& error, const size_t si) {
  // NOTE[loperdog] : êµ¬ì¡°ì²´ë? ë³´ë‚´??ê²ƒì„ ?•ì¸?ˆë‹¤.
  // ?ˆë¡œ??êµ¬ì¡°?ì„œ ?œë²ˆ???•ì¸???˜ê³  ?¨í‚· ?¸ë“¤ë§ì„ ?´ë³´??
  memcpy(&TestData, buffer_.data(), sizeof(Buffer));
  std::cout << "Client Server memcpy headersize : " << TestData.header << std::endl;
  std::cout << "Client Server memcpy size : " << TestData.size << std::endl;

  memcpy(&TestData, buffer_.data(), sizeof(Buffer));
  std::cout << "Server memcpy body : " << TestData.body.data() << std::endl;

  // TODO[loperdog] : ?œë²„ì²˜ë¦¬ë¥?ê¸°ë‹¤ë¦°ë‹¤.

  RecvHandling();
}

void app::TCPClient::RecvHandling() {
  std::cout << "Server : Start recv" << std::endl;
  memset(&buffer_, '\0', sizeof(buffer_));

  socket_.async_read_some(
    boost::asio::buffer(buffer_),
    boost::bind(&TCPClient::OnRecv, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred)
  );

}
//NOTE[loperdog] : ?„ì‹œë¡?ë§Œë“œ???ŒìŠ¤?¸ìš© ?ŒìŠ¤
void app::TCPClient::Connect(const std::string &ip, const uint16_t port) {

  boost::asio::ip::tcp::endpoint ep(
    boost::asio::ip::address::from_string(ip), port);

  // TODO: append timeout.
  socket_.async_connect(ep, 
    boost::bind(&app::TCPClient::OnConnected,this,
      boost::asio::placeholders::error));

}
//NOTE[loperdog] : ?´ê²ƒ???„ì‹œ
void app::TCPClient::OnConnected(const boost::system::error_code &err) {

  BOOST_ASSERT_MSG(err, "TCPClient connect err");

  RecvHandling();
}

void app::TCPClient::Close() {
  socket_.close();
}
/*

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
*/
