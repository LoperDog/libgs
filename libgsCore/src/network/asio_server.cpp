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

  // NOTE[loperdog] : 테스트용 콜백
  void NullCallBack(const boost::shared_ptr<Data> &Buffer) {
  }
}
// 임시로 만든다.
const app::TCPServer::TestFunction app::TCPServer::kNullCallBack = NullCallBack;

void app::TCPServer::ChatEvent(boost::shared_ptr<Data> Buffer) {
  BOOST_ASSERT(Buffer);
  std::cout << "TestChat in Server : " << Buffer->size << std::endl;


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
    boost::bind(&TCPServer::OnAccepted, shared_from_this(),
      client,boost::asio::placeholders::error
    ));
}

void app::TCPServer::OnAccepted(boost::shared_ptr<TCPClient> client,
  const boost::system::error_code &err) {

  AsyncAccept();

  boost::shared_ptr<libgs::Uuid> newuuid = boost::make_shared<libgs::Uuid>();
  BOOST_ASSERT(newuuid);
  
  userlist.insert(std::make_pair(newuuid, client));

  //NOTE[loperdog] : client set Recv
  //typedef boost::function<void(const boost::shared_ptr<Buffer> &buffer)> ReceiveCb;
  //ReceiveCb recv = bind(&TCPServer::ChatEvent,
  //  shared_from_this(), _1);

  client->RecvHandling(boost::bind(
    &TCPServer::ChatEvent,shared_from_this(),_1));
}
// NOTE[loperdog] : 소켓은 전역으로 선언된 IOservice에 연결한 뒤 생성된 소켓을 반환한다. 
app::TCPClient::TCPClient() : socket_(CreateTCPSocket()) {}

void app::TCPClient::OnSend(const boost::system::error_code & error) {

}

void app::TCPClient::SendHandling() {

}

void app::TCPClient::OnRecv(const boost::system::error_code& error,
  const size_t si, const Callback cb) {

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
  boost::shared_ptr<Data> toServerData =
    boost::make_shared<Data>(TestData);

  cb(toServerData);


  RecvHandling(cb);
}

void app::TCPClient::RecvHandling(const Callback cb) {
  std::cout << "Server : Start recv" << std::endl;
  memset(&buffer_, '\0', sizeof(buffer_));
  BOOST_ASSERT(cb);
  socket_.async_read_some(
    boost::asio::buffer(buffer_),
    boost::bind(&TCPClient::OnRecv, shared_from_this(),
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred,cb)
  );

}

void app::TCPClient::Close() {
  socket_.close();
}