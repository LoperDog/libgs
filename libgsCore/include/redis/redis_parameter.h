#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>


namespace libgs {

namespace redis {

struct RedisEndPoint {
  RedisEndPoint(const std::string& _address, const unsigned short _port)
      : address(boost::asio::ip::address::from_string(_address)),
        port(_port) {
  }

  boost::asio::ip::address address;
  const unsigned short port;
};

}  // namespace redis

}  // namespace libgs