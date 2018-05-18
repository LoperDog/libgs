#ifndef ASIO_SERVER_SERVER_H
#define ASIO_SERVER_SERVER_H

#include <boost/enable_shared_from_this.hpp>

#include "common.h"
#include "uuid.h"
#include "USession.h"

#include <boost/asio/buffer.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace app {
  class TCPServer : public boost::enable_shared_from_this<TCPServer>
  {
  public:
    typedef boost::function<void(
      const boost::shared_ptr<Buffer> &buffer,
      const boost::system::error_code &/*error*/,
      const size_t /*bytes_transferred*/)> Callback;
 
    //void OnRecved
    TCPServer() {}

    void OnSend(boost::shared_ptr<USession> session_);

    void OnRecv(boost::shared_ptr<USession> session_);

  private:
    void RecvHandling(const boost::system::error_code& error, const size_t si);

    void SendHandling(const boost::system::error_code & error);

    bool is_connect_;
    Buffer buffer_;

    Data test;
  };
  
  class TCPClient : public boost::enable_shared_from_this<TCPClient> 
  {
  public :
    TCPClient() : socket_(ioservice_),
      ep_(boost::asio::ip::address::from_string(SERVER_IP), SERVER_PORT) {
      Connect();
    }
    
    void OnRecv();

    void Connect()
    {
      socket_.async_connect(ep_,
        boost::bind(&TCPClient::handle_connect,this,
          boost::asio::placeholders::error
        )
      );
      OnRecv();
      ioservice_.run();
    }

  private : 
    void RecvHandling(const boost::system::error_code& error, const size_t si);
   
    void handle_connect(const boost::system::error_code& error)
    {
      boost::lock_guard<boost::mutex> guard(mutex_);
      if (error)
      {
        std::cout << "connect failed : " << error.message() << std::endl;
      }
      else
      {
        std::cout << "connected" << std::endl;
        testSend();
      }
    }

    void testSend();

    boost::asio::io_service ioservice_; 
    TCPSocket socket_;
    TCPEndPoint ep_;

    boost::mutex mutex_;

    Buffer buffer_;

    Data TestData;
  };
}
#endif // !ASIO_SERVER_SERVER_H
