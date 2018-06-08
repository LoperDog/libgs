#ifndef ASIO_SERVER_SERVER_H
#define ASIO_SERVER_SERVER_H

#include <boost/enable_shared_from_this.hpp>

#include "common.h"
#include "common/uuid.h"
#include "network/USession.h"

#include <boost/asio/buffer.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace app {
  class TCPClient;

  class TCPServer : public boost::enable_shared_from_this<TCPServer>
  {
  public:
    typedef boost::function<void(
      const boost::shared_ptr<Buffer> &buffer,
      const boost::system::error_code &/*error*/,
      const size_t /*bytes_transferred*/)> Callback;
 
    TCPServer(const Property &property) : property_(property) {}
    
    void Start();
    void AsyncAccept();

  private:

    void OnAccepted(const boost::shared_ptr<TCPClient> &client,
      const boost::system::error_code &err);

    // NOTE[loperdog] : 현재는 Session대신 클라이언트를 uuid와 묶어 저장한다.
    std::map<boost::shared_ptr<libgs::Uuid>, boost::shared_ptr<app::TCPClient>> userlist;

    Property property_;

    boost::shared_ptr<TCPAsyncAcceptor> acceptor_;

    // NOTE[loperdog] : 임시로 사용한다.
    Packet RecvPacket;
    Data RecvData;
    Data test;
  };
  
  class TCPClient : public boost::enable_shared_from_this<TCPClient> 
  {
  public :
    TCPClient();
    TCPSocket& Socket() { return socket_; }

    void RecvHandling();
    void SendHandling();

    void PassData(Data _data);

    void Close();

  private : 
    void OnRecv(const boost::system::error_code& error, const size_t si);
    void OnSend(const boost::system::error_code & error);

    void TestMethod(char* buffer, int len);

    TCPSocket socket_;

    boost::mutex mutex_;

    boost::shared_ptr<Data> dataBuffer_;
    Data* dataBuffer_t = nullptr;
    char* buffer;
    int stack = 0;
    Buffer buffer_;
    Data TestData;
    char localBuffer_[1024];
    int length = 0;
  };

  void InitializeAsio();
}
#endif // !ASIO_SERVER_SERVER_H
