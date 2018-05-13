#ifndef ASIO_SERVER_SERVER_H
#define ASIO_SERVER_SERVER_H

#include "common.h"
#include "uuid.h"

#include <boost/enable_shared_from_this.hpp>

namespace app {
  // 서버 속성
  struct Property {
    Property(const std::string &_ip, const uint16_t _port, const bool is_tcp)
      : ip(_ip), port(_port), istcp(is_tcp) {}
    std::string ip;
    uint16_t port;
    bool istcp;
  };
  
  class TCPServer : public boost::enable_shared_from_this<TCPServer>
  {
  public:
    typedef boost::function<void(
      const boost::shared_ptr<Buffer> &buffer,
      const boost::system::error_code &/*error*/,
      const size_t /*bytes_transferred*/)> Callback;
 
    //void OnRecved
    TCPServer(){}
    
    void TcpSend() {

    }
    void StartRecv(const boost::shared_ptr<USession> &session_) {
      memset(&buffer_, '\0', sizeof(buffer_));
      //boost::make_shared<USession>(session_)->Socket();
      
      //session->Socket().async_read_some(

      //);
      session_->Socket();
    }
  private:
    void ReadSend() {

    }
    bool is_connect_;
    Buffer buffer_;
  };
  
  class TCPClient : public boost::enable_shared_from_this<TCPClient> 
  {
  public :
    TCPClient() : socket_(ioservice_) , 
      ep_(boost::asio::ip::address::from_string(SERVER_IP), SERVER_PORT)
    {
      Connect();
    }

    void Connect()
    {
      socket_.async_connect(ep_,
        boost::bind(&TCPClient::handle_connect,this, boost::asio::placeholders::error
        )
      );
      ioservice_.run();
    }
   
  private : 
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
      }
    }

    boost::asio::io_service ioservice_; 
    TCPSocket socket_;
    TCPEndPoint ep_;

    boost::mutex mutex_;
  };
}
#endif // !ASIO_SERVER_SERVER_H
