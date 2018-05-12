#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "common.h"
#include "asio_server.h"
#include "USession.h"
#include "uuid.h"
#include <queue>

namespace app {
  class Server
  {
  public :

    void StartServer(const boost::asio::io_service& _io);
    
    Server(Property& _property) : 
      acceptor_(ioservice_,TCPEndPoint(boost::asio::ip::tcp::v4(),_property.port))
    {
      StartServer(ioservice_);
    }
    void OnRecive() {

    }
    // ����Ʈ ���� ���� ����.
    void PostAccept() 
    {
      std::cout << "ready to accept" << std::endl;
      
      object * newuuid = new object();
      USession * user = new USession(acceptor_.get_io_service());
      uuidlist.push(newuuid);

      userlist.insert(std::make_pair(newuuid, user));

      acceptor_.async_accept(userlist[newuuid]->Socket(),
        boost::bind(&Server::Handle_accept,
          this, user, boost::asio::placeholders::error));
    }

    void CloseSession() 
    {

    }
  private :
    void Handle_accept(USession * session, const boost::system::error_code& error) {
      if (!error) {
        std::cout << "���� ����" << std::endl;
        PostAccept();
      }
    }

    boost::shared_ptr<TCPServer> server_;
    boost::mutex mutex_;
    
    boost::asio::io_service ioservice_;
    TCPAsyncAcceptor acceptor_;

    // uuid�� ���� ������ ��������Ѵ�.
    std::map<object*, USession*> userlist;
    // uuid�� �����Ѵ�
    std::queue<object*> uuidlist;

  };
}

#endif // !SERVER_SERVER_H
