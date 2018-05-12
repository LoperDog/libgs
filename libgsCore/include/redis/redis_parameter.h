#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>


namespace libgs {
namespace Redis {

struct RedisEndPoint
{
  boost::asio::ip::address address;
  const unsigned short port;

  RedisEndPoint(const std::string& _address, const unsigned short _port) :
    address(boost::asio::ip::address::from_string(_address)),
    port(_port) { }
};

}
}