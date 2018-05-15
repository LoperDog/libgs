#include "Server.h"

// 서버를 시작한다
void app::Server::StartServer(const boost::asio::io_service& _io) {
  
  PostAccept();

  ioservice_.run();
  /*
  boost::lock_guard<boost::mutex> guard(mutex_);
  // 서버 프로퍼티를 넘겨준다.
  //boost::asio::io_service = ioservice_;
  //TCPSocket socket_test = ioservice_;
  //TCPServer *testServer = new  TCPServer();
  //std::cout << ioservice_.
  //server_->TEST1();
  server_ = boost::make_shared<TCPServer>();

  //server_->TEST1();
  */
}