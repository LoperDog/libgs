#include "Server.h"

// ������ �����Ѵ�
void app::Server::StartServer(const boost::asio::io_service& _io) {
  
  PostAccept();

  ioservice_.run();
  /*
  boost::lock_guard<boost::mutex> guard(mutex_);
  // ���� ������Ƽ�� �Ѱ��ش�.
  //boost::asio::io_service = ioservice_;
  //TCPSocket socket_test = ioservice_;
  //TCPServer *testServer = new  TCPServer();
  //std::cout << ioservice_.
  //server_->TEST1();
  server_ = boost::make_shared<TCPServer>();

  //server_->TEST1();
  */
}