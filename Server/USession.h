#ifndef USESSION_USESSUIN_H
#define USESSION_USESSUIN_H
#include "common.h"

class USession
{
public:
  //USession(const std::shared_ptr<TCPSocket>& socket) : socket_(socket) {}
  USession(boost::asio::io_service& socket) : socket_(socket) {}
  TCPSocket& Socket() { return socket_; }

private :
  TCPSocket socket_;
  //app::Data buffer_;
};


#endif // !USESSION_USESSUIN_H
