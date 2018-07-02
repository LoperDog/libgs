#include "stdafx.h"

#include "network/asio_server.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "network\rest.pb.h"

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
//const app::TCPServer::OnRecvCallBack app::TCPServer::kNullCallBack = NullCallBack;

void app::TCPServer::ClientOnRecv(const boost::shared_ptr<libgs::Uuid> uuid_,
  int64_t header_, char buffer_[]){

  // TODO[loperdog -> wonchae] : 이곳이 클라이언트에서 콜백을 시키는 곳.


  std::cout << "callback in thread" << std::endl;

  // NOTE[loperdog] : 이벤트로 들어온 콜백을 실행시킴
  std::string roomname{ "testroom" };
  Grouping(roomname.c_str(), uuid_);

  int sendType_ = eventList.find(header_)->second(uuid_, buffer_, sendData, NULL);
  eventList.find(app::MethodHeader::kTestSend)->second(uuid_, buffer_, sendData, sendType_);

  //auto group_ = FindGroup(uuid_, header_);
  //std::cout << "find join room name : " << joinroom << std::endl;
  //std::cout << "join user count : " << group[joinroom].size() << std::endl;
  //Send(uuid_);
}
std::list<boost::shared_ptr<libgs::Uuid>>* app::TCPServer::FindGroup
(boost::shared_ptr<libgs::Uuid> uuid_, int sendType_) {
  switch (sendType_)
  {
  case app::SendType::Non:
    return nullptr;
    break;
  case app::SendType::Self:
  {
    std::list<boost::shared_ptr<libgs::Uuid>>* returnData = new std::list<boost::shared_ptr<libgs::Uuid>>;
    returnData->push_back(uuid_);
    return returnData;
  }
  break;
  case app::SendType::Group:
    for (auto it = group.begin(); it != group.end(); it++) {
      for (auto jt = it->second.begin(); jt != it->second.end(); jt++) {
        if ((*jt)->ToString() == uuid_->ToString())
          return &it->second;
      }
    }
    break;
  case app::SendType::All:
  {
    std::list<boost::shared_ptr<libgs::Uuid>>* returnData = new std::list<boost::shared_ptr<libgs::Uuid>>;
    for (auto item : userlist) {
      returnData->push_back(item.first);
      return returnData;
    }
  }
    break;
  }
  
  //return group[uuid_];
  //return "";
}

void app::TCPServer::Send(const boost::shared_ptr<libgs::Uuid> uuid_) {
  //std::string groupname = FindGroup(uuid_);
  /*if (groupname.size() == 0)
    return;*/
}

void app::TCPServer::ChatEvent(boost::shared_ptr<Data> Buffer) {
  BOOST_ASSERT(Buffer);
  std::cout << "TestChat in Server : " << Buffer->data << std::endl;

  for (auto item : userlist) {
    item.second.get()->SendHandling(Buffer);
  }
}

// NOTE[loperdog] : 이벤트를 매핑한다.이는 서버 클래스에서 동작하며
// 메소드와 생성된 클래스 위치를 넘긴다.
int app::TCPServer::InsertRecvEvent(int64_t header, EventMethod ev/*, NetworkMethod fromclass*/) {
  //BOOST_ASSERT(fromclass.GetThis());
  
  // NOTE[loperdog] : 현재는 메소드만 넘겨 저장한다.
  eventList.insert(std::make_pair(header,
    boost::bind(ev, _1, _2, _3, _4)));

  return 0;
}

void app::TCPServer::Start() {
  TCPEndPoint ep(boost::asio::ip::tcp::v4(), property_.port);
  acceptor_ = boost::make_shared<TCPAsyncAcceptor>(
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
  
  // NOTE[loperdog] : 유저를 생성하고 tcpclient를 설정한다.
  auto tempuuid = server_.get()->CreateUser();
  server_.get()->GetUser(tempuuid)->SetTCPClient(client);

  client->RecvHandling(boost::bind(
    &TCPServer::ClientOnRecv,shared_from_this(),_1,_2,_3));
}

// NOTE[wonchae] : 그룹을 나눠서 넣어주는곳
void app::TCPServer::Grouping(const char roomname[],boost::shared_ptr<libgs::Uuid> uuid) {
  
  //std::string rn = roomname;
  //group.insert(std::make_pair(rn, uuid));
  group[roomname].push_back(uuid);
  std::cout << "user cnt : " << group[roomname].size() << std::endl;
  std::cout << "room cnt : " << group.size() << std::endl;

  std::cout << "Join Room Name : " << roomname << std::endl;
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
  const size_t si, const OnRecvCallBack cb) {

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
      TestMethod(buffer, stack, cb);
      //TODO[loperdog] : 콜백임시 테스트중 코드 수정바람.
      //cb(GetUuid(),1,buffer);
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

void app::TCPClient::TestMethod(char* buffer, int len, const OnRecvCallBack cb) {
  int stack = 0;

  while (len - stack > 0) {
    int size;
    memcpy(&size, buffer + stack, sizeof(size));
    Data localData;
    memcpy(&localData, buffer + stack, size);

    int bufSize = size - sizeof(Packet) - sizeof(Data::size);
    google::protobuf::io::ArrayInputStream is(localData.data, localData.bodySize);
    Login login;
    login.ParseFromZeroCopyStream(&is);
    std::cout << login.nickname() << std::endl;

    cb(GetUuid(), localData.header, localData.data);
    stack += size;
  }
}

void app::TCPClient::RecvHandling(const OnRecvCallBack cb) {
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