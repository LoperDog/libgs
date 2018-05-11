#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <redisclient/redisasyncclient.h>

namespace libgs {
namespace Redis {

struct RedisEndPoint;
struct RedisConnectImpl;

class RedisConnection
{
 public:
  RedisConnection();
  ~RedisConnection();

  bool Connect(const std::string& address, const unsigned short port);
 private:
  std::shared_ptr<RedisConnectImpl> _pimpl;
};

}
}