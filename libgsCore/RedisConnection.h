#pragma once
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <redisclient/redisasyncclient.h>

namespace libgs {
namespace Redis {

struct RedisEndPoint;

class RedisConnection
{
public:
  RedisConnection(const RedisEndPoint& _endpoint);
  ~RedisConnection();

  bool Connect();
private:
  std::shared_ptr<RedisEndPoint> endpoint;
  boost::asio::io_service ioService;
  redisclient::RedisAsyncClient redis;
};

}
}