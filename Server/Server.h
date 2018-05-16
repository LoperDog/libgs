#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <queue>

#include "common.h"
#include "asio_server.h"
#include "USession.h"
#include "uuid.h"

namespace app {
  class Server
  {
  public :

    void StartServer(const boost::asio::io_service& _io);
    
    Server(Property& _property) : 
      acceptor_(ioservice_,
        TCPEndPoint(boost::asio::ip::tcp::v4(),_property.port)) {

      StartServer(ioservice_);
    }
    void OnRecive() {

    }
    // ����Ʈ ���� ���� ����.
    void PostAccept() 
    {
      std::cout << "ready to accept" << std::endl;
      boost::lock_guard<boost::mutex> g(mutex_);
      boost::shared_ptr<object> newuuid = boost::make_shared<object>();
      boost::shared_ptr<USession> user = boost::make_shared<USession>(acceptor_.get_io_service());
      
      uuidlist.push(newuuid);

      userlist.insert(std::make_pair(newuuid, user));

      acceptor_.async_accept(userlist[newuuid]->Socket(),
        boost::bind(&Server::Handle_accept,
          this, userlist[newuuid], boost::asio::placeholders::error)); 

      std::cout << "before the Fucking accept" << std::endl;
    }

    void CloseSession() 
    {

    }
  private :

    void Handle_accept(const boost::shared_ptr<USession> session, const boost::system::error_code& error) {
      boost::lock_guard<boost::mutex> g(mutex_);
      std::cout << "���� ��û���� ���� �޼ҵ� ȣ��" << std::endl;

      if (!error) {
        std::cout << "���� ����" << std::endl;
        PostAccept();
        server_->StartRecv(session/*->Socket()*/);
      }
    }

    boost::shared_ptr<TCPServer> server_;
    boost::mutex mutex_;
    
    boost::asio::io_service ioservice_;
    TCPAsyncAcceptor acceptor_;

    // uuid�� ���� ������ ��������Ѵ�.
    std::map<boost::shared_ptr<object>, boost::shared_ptr<USession>> userlist;
    // uuid�� �����Ѵ�
    std::queue<boost::shared_ptr<object>> uuidlist;

  };
}

#endif // !SERVER_SERVER_H
