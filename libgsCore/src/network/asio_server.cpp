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
  std::cout << "TestChat in Server : " << Buffer->data << std::endl;

  for (auto item : userlist) {
    item.second.get()->SendHandling(Buffer);
  }
}

// NOTE[loperdog] : 이벤트를 매핑한다.이는 서버 클래스에서 동작하며
// 메소드와 생성된 클래스 위치를 넘긴다.
int app::TCPServer::InsertRecvEvent(int64_t header, EventMethod ev, NetworkMethod fromclass) {
  BOOST_ASSERT(fromclass.GetThis());
  
  eventList.insert(std::make_pair(header,
    boost::bind(ev,_1,_2)));

  return 0;
}

void app::TCPServer::Start() {
  TCPEndPoint ep(boost::asio::ip::tcp::v4(), property_.port);
  acceptor_ = boost::make_shared<TCPAsyncAcceptor>(
    //ioservice,ep,this);
    ioservice,ep);

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
  // NOTE[loperdog] : 클라이언트에 자신의 uuid를 넣어둔다.
  client->SetUuid(newuuid);

  client->RecvHandling(boost::bind(
    &TCPServer::ChatEvent,shared_from_this(),_1));
}
// NOTE[loperdog] : 소켓은 전역으로 선언된 IOservice에 연결한 뒤 생성된 소켓을 반환한다. 
app::TCPClient::TCPClient() : socket_(CreateTCPSocket()) {}

void app::TCPClient::OnSend(const boost::system::error_code & error) {
  std::cout << "Doing Sending" << std::endl;
}

void app::TCPClient::SendHandling(boost::shared_ptr<Data> data) {
  BOOST_ASSERT(data);
  
  std::cout << data.get()->data << std::endl;

  socket_.async_write_some(
    boost::asio::buffer(data.get(),data->size),
    boost::bind(&TCPClient::OnSend,shared_from_this(),
      boost::asio::placeholders::error));

}

void app::TCPClient::OnRecv(const boost::system::error_code& error,
  const size_t si, const Callback cb) {

  boost::shared_ptr<char> test = boost::make_shared<char>(256);
  std::cout << strlen(test.get()) << std::endl;

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