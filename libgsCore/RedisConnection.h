#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <redisclient/redisasyncclient.h>

namespace libgs {
namespace Redis {

class RedisConnection
{
  boost::asio::ip::address address;
  const unsigned short port;

  boost::asio::io_service ioService;
  redisclient::RedisAsyncClient redis;
 public:
  RedisConnection(const std::string& _address, const unsigned short _port);
  ~RedisConnection();

  bool Connect();
};

}
}