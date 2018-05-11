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
  RedisConnection(const RedisEndPoint& _endpoint);
 public:
  ~RedisConnection();

  bool Connect();
 private:
  std::shared_ptr<RedisConnectImpl> _pimpl;
};

}
}