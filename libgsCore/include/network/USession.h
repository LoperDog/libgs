#ifndef USESSION_USESSUIN_H
#define USESSION_USESSUIN_H

#include "common.h"
#include <queue>
namespace app {

  // 서버 속성
  struct Property {
    Property(const std::string &_ip, const uint16_t _port, const bool is_tcp)
      : ip(_ip), port(_port), istcp(is_tcp) {}
    std::string ip;
    uint16_t port;
    bool istcp;
  };

  class USession
  {
  public:
    //USession(const std::shared_ptr<TCPSocket>& socket) : socket_(socket) {}
    USession(boost::asio::io_service& socket) : socket_(socket) {	}

    TCPSocket& Socket() { return socket_; }

    //void InputData(boost::shared_ptr<app::Data> data) {
    void InputData(boost::shared_ptr<std::string> data) {
      snedBuffer_.push(data);
    }

    std::queue <boost::shared_ptr<std::string>> getSendBufferList() { return snedBuffer_; }

  private:
    TCPSocket socket_;
    std::queue <boost::shared_ptr<std::string>> snedBuffer_;
    //std::queue <boost::shared_ptr<app::Data>> snedBuffer_;
    //app::Data buffer_;
  };
}

#endif // !USESSION_USESSUIN_H
