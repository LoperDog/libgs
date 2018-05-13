#ifndef ASIO_SERVER_SERVER_H
#define ASIO_SERVER_SERVER_H

#include <boost/enable_shared_from_this.hpp>

#include "common.h"
#include "uuid.h"
#include "USession.h"

namespace app {
  // ���� �Ӽ�
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
      std::cout << "Server : Start recv" << std::endl;
      memset(&buffer_, '\0', sizeof(buffer_));
      //boost::make_shared<USession>(session_)->Socket();
      
<<<<<<< HEAD
<<<<<<< HEAD
      //session->Socket().async_read_some(

      //);
      session_->Socket();
    }
  private:
    void ReadSend() {

=======
=======
>>>>>>> ff9471c... 억샙트는 완료
      session_->Socket().async_read_some(
        boost::asio::buffer(buffer_),
        boost::bind(&TCPServer::ReadSend, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred)
      );
    }
  private:
    void ReadSend(const boost::system::error_code& error, const size_t si) {
      std::cout << "doing recv!" << std::endl;
      std::cout << buffer_.data() << std::endl;
<<<<<<< HEAD
>>>>>>> 931a35c... WIP:리시브 작업중
=======
>>>>>>> ff9471c... 억샙트는 완료
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
        boost::bind(&TCPClient::handle_connect,this,
          boost::asio::placeholders::error
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
        // Send Test();
        testSend();
      }
    }
    void testSend() {
      boost::asio::async_write(
        socket_,
        boost::asio::buffer("tqwotml"),
        [this](boost::system::error_code ec, std::size_t) 
      {
        if (!ec) {

          std::cout << "client Send No err" << std::endl;
        }
        else {
          std::cout << "client Send Error" << std::endl;
        }
      });
    }

    boost::asio::io_service ioservice_; 
    TCPSocket socket_;
    TCPEndPoint ep_;

    boost::mutex mutex_;
  };
}
#endif // !ASIO_SERVER_SERVER_H
