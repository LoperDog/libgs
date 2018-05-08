#ifndef TCPSERVER_H
#define TCPSERVER_H

#include"common.h"
#include <vector>
#include <queue>
#include <mutex>

#include"TestSession.h"

class TCPServer
{
public:
	// 생성과 동시에 억셉터를 생성한다.
	TCPServer(asio_service& core) ;
	void Init(const int nMaxSessionCount)
	{
			for (int i = 0; i < nMaxSessionCount; ++i)
			{
				TestSession* pSession = new TestSession(i,Myacceptor.get_io_service());
				
				m_SessionList.push_back(pSession);

				m_SessionQueue.push(i);

			}
			PostAccept();
	}
private:
	// 
	bool PostAccept()
	{
		if (m_SessionQueue.empty())
		{
			m_bIsAccepting = false;
			return false;
		}
		m_bIsAccepting = true;

		std::lock_guard<std::mutex> guard(m_Session_mutex);
		int nSessionID = m_SessionQueue.front();
		m_SessionQueue.pop();


		Myacceptor.async_accept(m_SessionList[nSessionID]->Socket(),
			boost::bind(&TCPServer::handle_accept,
				this, m_SessionList[nSessionID],
				boost::asio::placeholders::error));
	}
	void CloseSession(const int nSessionID)
	{
		std::cout << "클라이언트 접속 종료. 세션 ID: " << nSessionID << std::endl;
		m_SessionList[nSessionID]->Socket().close();

		std::lock_guard<std::mutex> guard(m_Session_mutex);
		m_SessionQueue.push(nSessionID);

		if (m_bIsAccepting == false)
		{
			PostAccept();
		}
	}
	/*
	void StartAccept()
	{
		std::cout << "클라이언트 접속 대기....." << std::endl;
		//m_pSession = new Session(m_acceptor.get_io_service());

		Myacceptor.async_accept(m_Session->Socket(),
			boost::bind(&TCP_Server::handle_accept,
				this,
				m_Session,
				boost::asio::placeholders::error)
		);
	}*/
	void handle_accept(TestSession* pSession, const boost_error_handle& error)
	{
		if (!error)
		{
			std::cout << "클라이언트 접속 성공" << std::endl;
			pSession->PostRecv();
		}
	}
	asio_tcp_acceptor Myacceptor;

	std::mutex m_Session_mutex;
	std::vector<TestSession*> m_SessionList;

	std::queue<int> m_SessionQueue;

	bool m_bIsAccepting;
	int m_nSeqNumber;
};

#endif // !TCPSERVER_H
