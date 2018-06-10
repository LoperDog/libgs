#ifndef ASIO_SERVER_SERVER_H
#define ASIO_SERVER_SERVER_H

#include <boost/enable_shared_from_this.hpp>

#include "common.h"
#include "common/uuid.h"
#include "network/USession.h"
#include "network/networkmethod.h"

#include <boost/asio/buffer.hpp>
#include <boost/archive/binary_oarchive.hpp>


namespace app {
  class TCPClient;

  class TCPServer : public boost::enable_shared_from_this<TCPServer>
  {
  public:
    typedef boost::function<void(
      const boost::shared_ptr<Data> &buffer)> Callback;

    typedef boost::function<void(
      const boost::shared_ptr<libgs::Uuid> uuid_,int64_t header_, 
      const boost::shared_ptr<char> &buffer_)> OnRecvCallBack;

    // NOTE[loperdog] : recv event형식
    typedef boost::function<void(boost::shared_ptr<libgs::Uuid>, char *)> EventMethod;

    TCPServer(const Property &property) : property_(property) {}
    
    void Start();
    void AsyncAccept();

    // NOTE[loperdog] : 클라이언트 반환 함수. 추후 세션으로 반환 하더라도좋음.
    const boost::shared_ptr<TCPClient> FindClient(
      const boost::shared_ptr<libgs::Uuid> uuid) {
      return userlist.find(uuid)->second; 
    }

    // NOTE[loperdog] : 클라이언트 리시브 이벤트에서 받을 함수
    void ClientOnRecv(const boost::shared_ptr<libgs::Uuid> uuid_,
      int64_t header_, const boost::shared_ptr<char> buffer_);

    // NOTE[loperdog] : 임시 테스트용 이벤트
    void ChatEvent(const boost::shared_ptr<Data> Buffer);
    
    // NOTE[loperdog] : 이벤트 추가용 함수.
    int InsertRecvEvent(int64_t, EventMethod, NetworkMethod);
  private:

    void OnAccepted(boost::shared_ptr<TCPClient> client,
      const boost::system::error_code &err);
    // NOTE[loperdog] : 현재는 Session대신 클라이언트를 uuid와 묶어 저장한다.
    std::map<boost::shared_ptr<libgs::Uuid>, boost::shared_ptr<app::TCPClient>> userlist;

    Property property_;

    boost::shared_ptr<TCPAsyncAcceptor> acceptor_;
    
    // NOTE[loperdog] : 이벤트를 매핑한다. 이벤트가 일어난 클라이언트의 UUID,공유받은 버퍼.
    std::map<int64_t, EventMethod> eventList;

    // NOTE[loperdog] : 임시로 사용한다.
    static const OnRecvCallBack kNullCallBack;
    Packet RecvPacket;
    Data RecvData;
    Data test;
  };
  
  class TCPClient : public boost::enable_shared_from_this<TCPClient> 
  {
  public :
    typedef boost::function<void(
      const boost::shared_ptr<Data> &buffer)> Callback;

    typedef boost::function<void(
      const boost::shared_ptr<libgs::Uuid> uuid_, int64_t header_,
      const boost::shared_ptr<char> &buffer_)> OnRecvCallBack;

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
    void OnRecv(const boost::system::error_code& error, const size_t si, const Callback cb);
    void OnSend(const boost::system::error_code & error);

    void TestMethod(char* buffer, int len);

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

  void InitializeAsio();
}
#endif // !ASIO_SERVER_SERVER_H
