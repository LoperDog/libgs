#include "redis/redis_connection.h"

#include "stdafx.h"

#include <functional>
#include <thread>

#include <boost/assert.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <redisclient/redisasyncclient.h>

#include "redis/redis_parameter.h"



namespace libgs {

namespace redis {

namespace {

std::shared_ptr<boost::asio::io_service> the_io_service;
std::shared_ptr<std::thread> the_thread;

}  // unnamed namespace


struct RedisConnection::RedisConnectionImpl {
  RedisConnectionImpl()
      : raw_redis_async_client(*the_io_service) {
  }

  std::shared_ptr<RedisEndPoint> endpoint;
  redisclient::RedisAsyncClient raw_redis_async_client;
};


RedisConnection::RedisConnection()
    : pimpl_(std::make_unique<
          RedisConnection::RedisConnectionImpl>()) {
}


RedisConnection::~RedisConnection() {
}


void RedisConnection::Connect(const std::string& address,
                              const uint16_t port,
                              const ConnectCallback &cb) {
  boost::system::error_code ec;
  boost::asio::ip::address::from_string(address, ec);

  BOOST_ASSERT(ec == boost::system::errc::success);

  pimpl_->endpoint =
      std::make_shared<RedisEndPoint>(address, port);

  pimpl_->raw_redis_async_client.connect(
      pimpl_->endpoint->address, pimpl_->endpoint->port, cb);
}


// TODO(inkeun): Finalize 를 추가한다.
void InitializeRedisIoService() {
  the_io_service = std::make_shared<boost::asio::io_service>();

  // TODO(inkeun): 쓰레드 종료 과정을 추가로 확인하도록 만든다.
  auto run_function = [the_io_service]() {
    std::shared_ptr<boost::asio::io_service::work> work =
        std::make_shared<boost::asio::io_service::work>(
            *the_io_service);
    the_io_service->run();
  };

  the_thread = std::make_shared<std::thread>(run_function);
}

}  // namespace redis

}  // namespace libgs