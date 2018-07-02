#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H


#include <queue>

#include "network\common.h"
#include "network\asio_server.h"
#include "network\USession.h"
#include "common\uuid.h"

#include <boost/asio/buffer.hpp>
#include <boost/enable_shared_from_this.hpp>

// NOTE[loperdog] : 추후 TCPServer와 UDPServer를 내포한 서버클래스 구현
namespace app {

  typedef boost::function<void(boost::shared_ptr<libgs::Uuid> , char *)> EventMethod;

  class TCPServer;
  class UDPServer;

  class Server : public boost::enable_shared_from_this<Server>{
  public:

    void Start(const Property &property);

    int InsertMethod(MethodHeader, EventMethod , bool );

    std::map<boost::shared_ptr<libgs::Uuid>,
      boost::shared_ptr<User>> GetUserList() { return userlist; }

    // TODO[loperdog] : 유저를 추가하는 부분.
    boost::shared_ptr<libgs::Uuid> CreateUser();
    boost::shared_ptr<User> GetUser(boost::shared_ptr<libgs::Uuid> uuid) {
      return userlist.find(uuid)->second;
    }

    void TestSend(boost::shared_ptr<libgs::Uuid> uuid, char * data);
    boost::shared_ptr<Server> GetServer() { return shared_from_this(); }
  private:
    //NOTE[loperdog] : 삭제 예정
    void Init();
    // NOTE[loperdog] : 서버들을 포함하고 있는다.
    boost::shared_ptr<TCPServer> tcpserver_;
    boost::shared_ptr<UDPServer> udpserver_;
    //NOTE[loperdog] : 유저 정보를 가지고 있는다.
    std::map<boost::shared_ptr<libgs::Uuid>, boost::shared_ptr<User>> userlist;


  };
}

#endif // !SERVER_SERVER_H
