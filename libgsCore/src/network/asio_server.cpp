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

void app::TCPClient::PassData(Data _data) {

}

void app::TCPClient::OnRecv(const boost::system::error_code& error, const size_t si) {
  //Wonchae : Test를 위한 임시 변수;

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
  RecvHandling();

    // TODO[loperdog] : 서버의 온리시브를 실행한다.
}

void app::TCPClient::TestMethod(char* buffer, int len) {
  static int length;
  length += len;
  std::cout << "length : " << length << std::endl;
  static int cnt;
  std::cout << ++cnt << std::endl;
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