#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <queue>

#include "common.h"
#include "asio_server.h"
#include "USession.h"
#include "common/uuid.h"

namespace app {
  class Server
  {
  public :
    void StartServer(const boost::asio::io_service& _io);
    
    Server(Property& _property) : 
      acceptor_(ioservice_,
        TCPEndPoint(boost::asio::ip::tcp::v4(),_property.port)) {
			server_ = boost::make_shared<TCPServer>();
      StartServer(ioservice_);
    }
		void OnRecive() {}
    // 엑셉트 시작 전에 할일.
		void PostAccept();

		void SendtoClient();

    void CloseSession() 
    {
    }
  private :

		void Handle_accept(boost::shared_ptr<USession> session,
			const boost::system::error_code& error);

    boost::shared_ptr<TCPServer> server_;

		boost::mutex mutex_;
		boost::mutex mutex_1;
    
    boost::asio::io_service ioservice_;
    TCPAsyncAcceptor acceptor_;

    // uuid와 같이 세션을 저장관리한다.
    std::map<boost::shared_ptr<libgs::Uuid>, boost::shared_ptr<USession>> userlist;
    // uuid를 관리한다
    std::queue<boost::shared_ptr<libgs::Uuid>> uuidlist;

  };
}

#endif // !SERVER_SERVER_H
