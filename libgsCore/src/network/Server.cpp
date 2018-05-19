#include "stdafx.h"
#include "network/Server.h"

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

void app::Server::PostAccept(){

	std::cout << "ready to accept" << std::endl;
	boost::lock_guard<boost::mutex> g(mutex_);
	std::cout << "mutex" << std::endl;
	boost::shared_ptr<libgs::Uuid> newuuid = boost::make_shared<libgs::Uuid>();
	BOOST_ASSERT(newuuid);

	boost::shared_ptr<USession> user = boost::make_shared<USession>(acceptor_.get_io_service());
	BOOST_ASSERT(user);

	uuidlist.push(newuuid);
	std::cout << "push" << std::endl;

	userlist.insert(std::make_pair(newuuid, user));
	std::cout << "insert" << std::endl;

	acceptor_.async_accept(userlist[newuuid]->Socket(),
		boost::bind(&Server::Handle_accept,
			this, userlist[newuuid], boost::asio::placeholders::error));

}

void app::Server::Handle_accept(boost::shared_ptr<USession> session
	, const boost::system::error_code & error){
	std::cout << "���� ��û���� ���� �޼ҵ� ȣ��" << std::endl;

	if (!error) {
		std::cout << "���� ����" << std::endl;
		{
			boost::lock_guard<boost::mutex> g(mutex_1);
			server_->OnRecv(session/*->Socket()*/);
			
			boost::shared_ptr<std::string> testSendData =
				boost::make_shared<std::string>("Jeong Wonchae");

			for (auto i = userlist.begin(); i != userlist.end(); i++) {
				i->second->InputData(testSendData);
			}
			SendtoClient();
		}
	}
	PostAccept();
}

void app::Server::SendtoClient(){
	BOOST_ASSERT(!userlist.empty());

	for (auto i = userlist.begin(); i != userlist.end(); i++){
		if (!i->second->getSendBufferList().empty()) {
			server_->OnSend(i->second);
		}
	}
}