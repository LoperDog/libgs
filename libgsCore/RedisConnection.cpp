#include "stdafx.h"
#include "RedisConnection.h"
#include "RedisParameter.h"

namespace libgs {
namespace Redis {

struct RedisConnectImpl
{
  std::shared_ptr<RedisEndPoint> endpoint;
  boost::asio::io_service ioService;
  redisclient::RedisAsyncClient redis;
  RedisConnectImpl() : redis(ioService) {}
};

RedisConnection::RedisConnection(const RedisEndPoint& _endpoint) :
  _pimpl(std::make_shared<RedisConnectImpl>())  
{
  _pimpl->endpoint = std::make_shared<RedisEndPoint>(_endpoint);
}

RedisConnection::~RedisConnection()
{
}

bool RedisConnection::Connect()
{
  bool isSuccess = false;
  _pimpl->redis.connect(_pimpl->endpoint->address, _pimpl->endpoint->port, [this, &isSuccess](bool _isSuccess, std::string errcode) {
    isSuccess = _isSuccess;
  });

  return isSuccess;
}

}
}