// libgsCoreTest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"

#include "network/common.h"
#include "network/Server.h"
#include "network/asio_server.h"
#include <thread>


int main() {

  std::cout << "main start" << std::endl;

  app::Property p(SERVER_IP, SERVER_PORT, true);

  boost::shared_ptr<app::TCPServer> testServer =
    boost::make_shared<app::TCPServer>(p);

  testServer->Start();

  app::InitializeAsio();

  while (true) {

  }
}