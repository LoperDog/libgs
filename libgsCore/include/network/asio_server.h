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
 
    //void OnRecved
    //TCPServer() {}
    TCPServer(const Property &property) : property_(property) {}
    
    void Start();
    void AsyncAccept();

  private:

    void OnAccepted(const boost::shared_ptr<TCPClient> &client,
      const boost::system::error_code &err);
    /*
    void RecvHandling(boost::shared_ptr<USession> session_, const boost::system::error_code& error, const size_t si);

    void SendHandling(const boost::system::error_code & error);
    */

    // NOTE[loperdog] : ?�라?�언?�로 ?�재??묶어 ?�다.
    std::map<boost::shared_ptr<libgs::Uuid>, boost::shared_ptr<app::TCPClient>> userlist;

    Property property_;

    boost::shared_ptr<TCPAsyncAcceptor> acceptor_;


    // NOTE[loperdog] : ?�재???��????��?�?추후 ?�정 ?�정
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

    void Close();

    //NOTE[loperdog] : ?�시 ?�스?�용 ?�결?�다.
    void Connect(const std::string &ip, const uint16_t port);
    void OnConnected(const boost::system::error_code &err);

  private : 
    void OnRecv(const boost::system::error_code& error, const size_t si);
    void OnSend(const boost::system::error_code & error);

    //void testSend();

    TCPSocket socket_;

    boost::mutex mutex_;

    Buffer buffer_;
    Data TestData;
    boost::shared_ptr<Data> Buffer;
  };

  void InitializeAsio();
}
#endif // !ASIO_SERVER_SERVER_H
