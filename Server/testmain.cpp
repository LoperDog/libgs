#include "common.h"
#include "Server.h"
#include <thread>

int main() {
  std::cout << "���� ����" << std::endl;
  app::Property t(SERVER_IP, SERVER_PORT,true);
  app::Server First(t);

  std::thread th1([]() {
    Sleep(3);
    std::cout << "add client;" << std::endl;
    
    boost::shared_ptr<app::TCPClient> testClient =
      boost::make_shared<app::TCPClient>();
  });
  std::thread th2([]() {
    Sleep(3);
    std::cout << "add client;" << std::endl;

    boost::shared_ptr<app::TCPClient> testClient =
      boost::make_shared<app::TCPClient>();
  });
  std::thread th3([]() {
    Sleep(3);
    std::cout << "add client;" << std::endl;

    boost::shared_ptr<app::TCPClient> testClient =
      boost::make_shared<app::TCPClient>();
  });

  while (true) {

  }
}