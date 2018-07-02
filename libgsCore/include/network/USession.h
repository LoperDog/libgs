#ifndef USESSION_USESSUIN_H
#define USESSION_USESSUIN_H

#include "common.h"
#include <queue>
namespace app {
  class TCPClient;
  class UDPClient;
  // 서버 속성
  struct Property {
    Property(const std::string &_ip, const uint16_t _port, const bool is_tcp)
      : ip(_ip), port(_port), istcp(is_tcp) {}
    std::string ip;
    uint16_t port;
    bool istcp;
  };
  // 세션을 가지고 있는 유저 클래스를 만든다.
  class User {
  public :
    void SetTCPClient(boost::shared_ptr<TCPClient> client) { tcpclient_ = client; }
    void SetUDPClient(boost::shared_ptr<UDPClient> client) { udpclient_ = client; }

    boost::shared_ptr<TCPClient> GetTCPClient() { return tcpclient_; }
    boost::shared_ptr<UDPClient> GetUDPClient() { return udpclient_; }
  private :
    boost::shared_ptr<app::TCPClient> tcpclient_;
    boost::shared_ptr<app::UDPClient> udpclient_;
  };
  class USession
  {
  public:
    USession(boost::asio::io_service& socket) : socket_(socket) {	}

    TCPSocket& Socket() { return socket_; }

    void InputData(boost::shared_ptr<std::string> data) {
      snedBuffer_.push(data);
    }

    std::queue <boost::shared_ptr<std::string>> getSendBufferList() { return snedBuffer_; }

  private:
    TCPSocket socket_;
    std::queue <boost::shared_ptr<std::string>> snedBuffer_;
  };
}

#endif // !USESSION_USESSUIN_H
