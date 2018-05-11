#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <redisclient/redisasyncclient.h>

namespace libgs {
namespace Redis {

class RedisConnection
{
 public:
  RedisConnection();
  ~RedisConnection();

  bool Connect(const std::string& address, const unsigned short port);
 private:
  struct RedisConnectionImpl;
  std::shared_ptr<RedisConnectionImpl> pimpl_;
};

}
}