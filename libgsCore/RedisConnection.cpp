#include "stdafx.h"
#include "RedisConnection.h"
#include "RedisParameter.h"

namespace libgs {
namespace Redis {

struct RedisConnection::RedisConnectionImpl
{
  std::shared_ptr<RedisEndPoint> endpoint;
  boost::asio::io_service ioService;
  redisclient::RedisAsyncClient redis;
  RedisConnectionImpl() : redis(ioService) {}
};

RedisConnection::RedisConnection() :
  pimpl_(std::make_shared<RedisConnection::RedisConnectionImpl>())
{
}

RedisConnection::~RedisConnection()
{
}

bool RedisConnection::Connect(const std::string& address, const unsigned short port)
{
  boost::system::error_code ec;
  boost::asio::ip::address::from_string(address, ec);

  if (ec)
  {
    std::cout << "Invalid Address, errcode : " << ec << std::endl;
    return false;
  }

  pimpl_->endpoint = std::make_shared<RedisEndPoint>(address, port);

  bool isSuccess = false;
  pimpl_->redis.connect(pimpl_->endpoint->address, pimpl_->endpoint->port, [&isSuccess](bool _isSuccess, std::string errcode) {
    isSuccess = _isSuccess;
    std::cout << isSuccess << std::endl;
  });

  return isSuccess;
}

}
}