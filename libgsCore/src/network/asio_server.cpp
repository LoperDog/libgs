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
void app::TCPServer::Start() {
  TCPEndPoint ep(boost::asio::ip::tcp::v4(), property_.port);
  acceptor_ = boost::make_shared<TCPAsyncAcceptor>(
    ioservice,ep,this);

  BOOST_ASSERT(acceptor_);

  AsyncAccept();

}

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

  boost::shared_ptr<libgs::Uuid> newuuid = boost::make_shared<libgs::Uuid>();
  BOOST_ASSERT(newuuid);
  
  userlist.insert(std::make_pair(newuuid, client));

  //NOTE[loperdog] : client set Recv
  client->RecvHandling();
}
// NOTE[loperdog] : 소켓은 전역으로 선언된 아이오 서비스에 연결한 뒤 생성된 소켓을 반환한다. 
app::TCPClient::TCPClient() : socket_(CreateTCPSocket()) {}

void app::TCPClient::OnSend(const boost::system::error_code & error) {

}

void app::TCPClient::SendHandling() {

}

void app::TCPClient::OnRecv(const boost::system::error_code& error, const size_t si) {

  //Packet testPacket;
  //Wonchae : 임시 리시브 코드
  memcpy(&TestData, buffer_.data(), sizeof(Buffer));
  std::cout << "Client Server memcpy headersize : " << TestData.header << std::endl;
  std::cout << "Client Server memcpy size : " << TestData.size << std::endl;

  if (si == 0) {
    Close();
    std::cout << "Client disconnect" << std::endl;
    return ;
  }

  std::cout << "size : " << sizeof(Buffer) << std::endl;

  size_t size = sizeof(Packet) + sizeof(TestData.bodySize);
  size_t addSize = TestData.size - size;
  {
    boost::lock_guard<boost::mutex> guard(mutex_);

    char* msgbuf = new char[TestData.bodySize]{'\0',};

    std::cout << "new data size : " << TestData.bodySize << std::endl;
    memcpy(msgbuf, buffer_.data() + size, addSize);

    std::cout << "msgbuf size : " << std::strlen(msgbuf) << std::endl;
    std::cout << "Server memcpy body : " << msgbuf << std::endl;

    delete []msgbuf;
  }
  
  // TODO[loperdog] : 서버의 온리시브를 실행한다.

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

void app::TCPClient::Close() {
  socket_.close();
}