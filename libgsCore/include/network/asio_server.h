#ifndef ASIO_SERVER_SERVER_H
#define ASIO_SERVER_SERVER_H

#include <boost/enable_shared_from_this.hpp>

#include "common.h"
#include "network\Server.h"
#include "common/uuid.h"
#include "network/USession.h"
#include "network/networkmethod.h"

#include <boost/asio/buffer.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace app {

  typedef boost::function<void(boost::shared_ptr<libgs::Uuid>, char *)> EventMethod;

  class TCPClient;
  class Server;

  class TCPServer : public boost::enable_shared_from_this<TCPServer>
  {
  public:
    typedef boost::function<void(
      const boost::shared_ptr<Data> &buffer)> Callback;

    typedef boost::function<void(
      const boost::shared_ptr<libgs::Uuid> uuid_,int64_t header_, 
      char buffer_[])> OnRecvCallBack;

    // NOTE[loperdog] : recv event형식
    typedef boost::function<int(boost::shared_ptr<libgs::Uuid>
      , char *, std::map<const boost::shared_ptr<libgs::Uuid> , const  char *> &
      , int sendType_)> EventMethod;

    TCPServer(const Property &property) : property_(property) {}
    
    TCPServer(const Property &property, 
      const boost::shared_ptr<Server> server) 
      : property_(property),  server_(server){}

    void Start();
    void AsyncAccept();

    // NOTE[loperdog] : 클라이언트 반환 함수. 추후 세션으로 반환 하더라도좋음.
    const boost::shared_ptr<TCPClient> FindClient(
      boost::shared_ptr<libgs::Uuid> uuid) {
      return userlist.find(uuid)->second; 
    }

    // NOTE[loperdog] : 클라이언트 리시브 이벤트에서 받을 함수
    void ClientOnRecv(const boost::shared_ptr<libgs::Uuid> uuid_,
      int64_t header_, char buffer_[]);

    // NOTE[loperdog] : 임시 테스트용 이벤트
    void ChatEvent(const boost::shared_ptr<Data> Buffer);
    std::map<boost::shared_ptr<libgs::Uuid>, boost::shared_ptr<app::TCPClient>> GetUserList() {
      return userlist;
    }
    // NOTE[loperdog] : 이벤트 추가용 함수.
    int InsertRecvEvent(int64_t, EventMethod /*, NetworkMethod fromclass*/);

    const std::map<const boost::shared_ptr<libgs::Uuid>, const  char *>*
      GetSendData() { return &sendData; }
    void DeleteSendData(const boost::shared_ptr<libgs::Uuid> uuid_){ sendData.erase(uuid_);}

    std::list<boost::shared_ptr<libgs::Uuid>>* FindGroup
    (boost::shared_ptr<libgs::Uuid> uuid_, int sendType_);

    //std::list<libgs::Uuid> GetGroupUser(std::string groupname);
  private:

    void Grouping(const char roomname[],boost::shared_ptr<libgs::Uuid> uuid);

    void Send(const boost::shared_ptr<libgs::Uuid> uuid_);

    void OnAccepted(boost::shared_ptr<TCPClient> client,
      const boost::system::error_code &err);
    boost::shared_ptr<app::Server> server_;
    
    Property property_;

    boost::shared_ptr<TCPAsyncAcceptor> acceptor_;
    
    // NOTE[loperdog] : 이벤트를 매핑한다. 이벤트가 일어난 클라이언트의 UUID,공유받은 버퍼.
    std::map<int64_t, EventMethod> eventList;

    // NOTE[loperdog] : 임시로 사용한다.
    static const OnRecvCallBack kNullCallBack;
    Packet RecvPacket;
    Data RecvData;
    Data test;

    std::map<std::string, std::list<boost::shared_ptr<libgs::Uuid>>> group;
    std::map<const boost::shared_ptr<libgs::Uuid>, const  char *> sendData;
  };
  
  class TCPClient : public boost::enable_shared_from_this<TCPClient> 
  {
  public :
    typedef boost::function<void(
      const boost::shared_ptr<Data> &buffer)> Callback;

    typedef boost::function<void(
      const boost::shared_ptr<libgs::Uuid> uuid_, int64_t header_,
      char buffer[])> OnRecvCallBack;

    //NOTE[loperdog] : 테스트용 펑션
    typedef boost::function<void(const boost::shared_ptr<Data> &Buffer)> TestFunction;

    TCPClient();
    TCPSocket& Socket() { return socket_; }

    void RecvHandling(const OnRecvCallBack cb);
    void SendHandling(boost::shared_ptr<Data> data);

    void PassData(Data _data);

    void Close();

    // NOTE [loperdog] : 클라이언트가 자신임을 알수 있도록
    void SetUuid(const boost::shared_ptr<libgs::Uuid> id) { MyUuid = id; }
    const boost::shared_ptr<libgs::Uuid> GetUuid() { return MyUuid; }
  private : 
    void OnRecv(const boost::system::error_code& error, const size_t si, const OnRecvCallBack cb);
    void OnSend(const boost::system::error_code & error);

    void TestMethod(char* buffer, int len,const OnRecvCallBack cb);

    TCPSocket socket_;

    boost::shared_ptr<libgs::Uuid> MyUuid;

    boost::mutex mutex_;

    boost::shared_ptr<Data> dataBuffer_;
    Data* dataBuffer_t = nullptr;
    char* buffer;
    int stack = 0;

    Buffer buffer_;
    Data TestData;
    char localBuffer_[1024];
    int length = 0;
  };
  //NOTE [loperdog] : UDP 모듈 추가중...
  class UDPClient;

  class UDPServer {

  };
  class UDPClient {

  };

  void InitializeAsio();
}
#endif // !ASIO_SERVER_SERVER_H
