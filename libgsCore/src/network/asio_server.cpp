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

  static int cnt = 0;
  {
    boost::lock_guard<boost::mutex> guard(mutex_);
    std::cout << "cnt : " << ++cnt << std::endl;
  }

  if (si == 0) {
    Close();
    std::cout << "Client disconnect" << std::endl;
    return;
  }
  else {
    puts("-------------------------------------------------");

    size_t size = 0;
    int isize = sizeof(size);

    char* lo_buffer = nullptr;
    {

      boost::lock_guard<boost::mutex> guard(mutex_);

      if (length != 0) {
        lo_buffer = new char[length];

        memcpy(lo_buffer, buffer, length);
        delete[]buffer;
      }

      buffer = new char[si + length];

      if (lo_buffer != nullptr)
      {
        memcpy(buffer, lo_buffer, length);

        delete[]lo_buffer;
      }
      memcpy(buffer + length, buffer_.data(), si);
    }

    while (isize + stack < si + length)
    {
      std::cout << "stack : " << stack << std::endl;

      memcpy(&size, buffer + stack, isize);
      std::cout << "size : " << size << std::endl;

      stack += size;
    }

    if (stack == si + length) {
      boost::lock_guard<boost::mutex> guard(mutex_);
      TestMethod(buffer, stack);
      length = 0;
      stack = 0;
    }
    else{
      length += si;
    }

  }
  RecvHandling(cb);

    // TODO[loperdog] : 서버의 온리시브를 실행한다.
}

void app::TCPClient::TestMethod(char* buffer, int len) {
  static int length;
  length += len;
  std::cout << "length : " << length << std::endl;
  static int cnt;
  std::cout << ++cnt << std::endl;
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