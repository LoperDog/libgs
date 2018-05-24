#pragma once

#include <functional>
#include <string>
#include <memory>


namespace libgs {

namespace redis {

class RedisConnection {
 public:
  using ConnectCallback =
      std::function<void(const bool /*success*/,
                         const std::string& /*error_message*/)>;

  RedisConnection();
  ~RedisConnection();

  void Connect(const std::string& address, const uint16_t port,
               const ConnectCallback &cb);

 private:
  struct RedisConnectionImpl;
  std::unique_ptr<RedisConnectionImpl> pimpl_;
};


void InitializeRedisIoService();

}  // namespace redis

}  // namespace libgs