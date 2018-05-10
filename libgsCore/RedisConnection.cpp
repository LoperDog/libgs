#include "stdafx.h"
#include "RedisConnection.h"
#include "RedisParameter.h"

namespace libgs {
namespace Redis {

RedisConnection::RedisConnection(const RedisEndPoint& _endpoint) :
  endpoint(std::make_shared<RedisEndPoint>(_endpoint)),
  redis(ioService)
{
}

RedisConnection::~RedisConnection()
{
}

bool RedisConnection::Connect()
{
  redis.connect(endpoint->address, endpoint->port, [this](bool isSuccess, std::string errcode) {
    if (!isSuccess)
    {
      return false;
    }
  });

  return true;
}

}
}