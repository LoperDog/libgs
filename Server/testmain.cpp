#include "common.h"
#include "Server.h"
#include <thread>

int main() {
  std::cout << "서버 시작" << std::endl;
  app::Property t(SERVER_IP, SERVER_PORT,true);
  app::Server First(t);

  std::thread th([]() {
    Sleep(3000);
    std::cout << "add client;" << std::endl;
    
    boost::shared_ptr<app::TCPClient> testClient =
      boost::make_shared<app::TCPClient>();
  });

  while (true) {

  }
}