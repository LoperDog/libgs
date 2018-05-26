#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <queue>

#include "common.h"
#include "asio_server.h"
#include "USession.h"
#include "common/uuid.h"
/*
namespace app {
  class Server
  {
  public :
    void StartServer(const boost::asio::io_service& _io);

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
    
    TCPAsyncAcceptor acceptor_;

  };
}

*/
#endif // !SERVER_SERVER_H
