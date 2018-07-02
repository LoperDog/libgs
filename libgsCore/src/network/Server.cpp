#include "stdafx.h"
#include "network/Server.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "network\rest.pb.h"

//
void app::Server::Start(const Property &property) {
  // Note[loperdog] : 서버 생성부
  tcpserver_ = boost::make_shared<app::TCPServer>(property,shared_from_this());

  // NOTE[loperdog] : 생성후 이벤트를 등록 시킨다.
  //Init();
  
  tcpserver_.get()->Start();
}

int LoginData(boost::shared_ptr<libgs::Uuid> uuid_, char * data_
  , std::map< const boost::shared_ptr<libgs::Uuid>, const char *> &sendData_) {


  size_t len = std::strlen(data_);
  google::protobuf::io::ArrayInputStream is(data_, len);
  Login login;
  login.ParseFromZeroCopyStream(&is);
  std::cout << login.nickname() << std::endl;

  std::cout << "this is Insert LoginFuntion" << std::endl;
  //sendData[uuid_.get()] = login.nickname().c_str();
  //sendData_.insert(std::make_pair(uuid_, login.nickname().c_str()));

  return app::SendType::Non;
}
int RoomListData(boost::shared_ptr<libgs::Uuid> uuid_, char * data_
  , std::map<const boost::shared_ptr<libgs::Uuid> , const  char *> &sendData_) {
  Roomlist rl;
  std::cout << "Room List Request Data in" << std::endl;
  std::cout << data_ << std::endl;
  sendData_.insert(std::make_pair(uuid_, data_));
  return app::SendType::Self;
}
int RoomMakeData(boost::shared_ptr<libgs::Uuid>, char * data_
  , std::map<const boost::shared_ptr<libgs::Uuid>, const  char *> &sendData_) {
  std::cout << "this is InsertTestMethod" << std::endl;
  std::cout << data_ << std::endl;
  return app::SendType::All;
}
int RoomJoinData(boost::shared_ptr<libgs::Uuid>, char * data_
  , std::map<const boost::shared_ptr<libgs::Uuid> , const  char *> &sendData_) {
  std::cout << "this is InsertTestMethod" << std::endl;
  std::cout << data_ << std::endl;
  return app::SendType::All;
}
int ChatData(boost::shared_ptr<libgs::Uuid>, char * data_
  , std::map<const boost::shared_ptr<libgs::Uuid> , const  char *> &sendData_) {
  std::cout << "this is InsertTestMethod" << std::endl;
  std::cout << data_ << std::endl;
  return app::SendType::All;
}
// NOTE[lopperdog] : 임시로 만든 서버단의 센드 서버의 유저를 추가후 수정함.
int app::Server::TestSend(int sendType_) {
  auto sendData = tcpserver_->GetSendData();

  for (auto group = sendData->begin(); group != sendData->end();) {
    auto userlist = tcpserver_->FindGroup(group->first, sendType_);
    if (userlist == nullptr)
      return 0;

    for (auto user : *userlist) {
      boost::shared_ptr<Data> d = boost::make_shared<Data>();
      d->header = 0;

      memcpy(d->data, "testSend", strlen("testSend"));
      d->bodySize = strlen("testSend");
      d->size = sizeof(Packet) + sizeof(Data::size) + d->bodySize;
      tcpserver_->FindClient(user)->SendHandling(d);
    }

    //tcpserver_->GetSendData().erase(group.first);
    tcpserver_->DeleteSendData((group++)->first);
  }
  return 0;
}

void app::Server::Init() {
  //NOTE[loperdog] : 서버에서 작동할 이벤트를 매핑한다.
  tcpserver_.get()->InsertRecvEvent(app::MethodHeader::kLogin,
    boost::bind(&LoginData, _1, _2, _3));
  tcpserver_.get()->InsertRecvEvent(app::MethodHeader::kRoomList,
    boost::bind(&RoomListData, _1, _2, _3));
  tcpserver_.get()->InsertRecvEvent(app::MethodHeader::kRoomMake,
    boost::bind(&RoomMakeData, _1, _2, _3));
  tcpserver_.get()->InsertRecvEvent(app::MethodHeader::kRoomJoin,
    boost::bind(&RoomJoinData, _1, _2, _3));
  tcpserver_.get()->InsertRecvEvent(app::MethodHeader::kChat,
    boost::bind(&ChatData, _1, _2, _3));;
  tcpserver_.get()->InsertRecvEvent(app::MethodHeader::kTestSend,
    boost::bind(&app::Server::TestSend, this,_4));
}

int app::Server::InsertMethod(app::MethodHeader header, app::EventMethod method, bool istcp) {
  if (istcp) {
    tcpserver_->InsertRecvEvent(header,method);
  }
  else {

  }
  return 0;
}