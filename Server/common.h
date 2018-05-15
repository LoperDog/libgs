
#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4439


using TCPSocket = boost::asio::ip::tcp::socket;
using TCPAsyncAcceptor = boost::asio::ip::tcp::acceptor;
using TCPEndPoint = boost::asio::ip::tcp::endpoint;

typedef std::array<char, 256> Buffer;

namespace app {
  struct Packet {
    Packet(const size_t _size, const int64_t _header):
      size(_size), header(_header){}
    size_t size;
    int64_t header;
  };
  struct Data : Packet {
    std::array<char, 256> body;
  };
}
#endif