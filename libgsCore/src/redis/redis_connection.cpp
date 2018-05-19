#include "redis/redis_connection.h"

#include "stdafx.h"

#include <iostream>

#include <functional>
#include <mutex>
#include <thread>

#include <boost/assert.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/thread/barrier.hpp>
#include <redisclient/redisasyncclient.h>

#include "system/thread_pool.h"

#include "redis/redis_parameter.h"


namespace libgs {

namespace redis {

namespace {

const std::string kCommandSet("set");  // NOLINT(runtime/string)
const std::string kCommandZAdd("zadd");  // NOLINT(runtime/string)
const std::string kCommandZRange("zrange");  // NOLINT(runtime/string)

const RedisConnection::IntResponses kEmptyIntResponses;
const RedisConnection::StringResponses kEmptyStringResponses;

std::mutex the_mutex;
std::shared_ptr<ThreadPool> the_redis_thread_pool;


const RedisConnection::Error GetError(
    const redisclient::RedisValue &value) {
  return RedisConnection::Error(
      (value.isOk() && !value.isError()), value.isNull());
}

}  // unnamed namespace


RedisConnection::Error::Error(const bool _success, const bool _is_null)
    : success(_success), is_null(_is_null) {
}


class RedisConnection::Impl {
 public:
  using ConnectCallback = RedisConnection::ConnectCallback;
  using IntResponse = RedisConnection::IntResponse;
  using IntResponses = RedisConnection::IntResponses;
  using StringResponse = RedisConnection::StringResponse;
  using StringResponses = RedisConnection::StringResponses;

  Impl();
  ~Impl();

  void Connect(const std::string &address, const uint16_t port,
               const ConnectCallback &cb);

  // Set [
  void Set(const std::string &key, const int64_t value,
           const IntResponseCallback &cb);

  void Set(const std::string &key, const std::string &value,
           const StringResponseCallback &cb);
  // ]

  // SortedSet [
  void ZAdd(const std::string &key, const int64_t score,
            const int64_t value, const ResponseCallback &cb);

  void ZAdd(const std::string &key, const int64_t score,
            const std::string &value, const ResponseCallback &cb);

  void ZRange(const std::string &key, const size_t start,
              const size_t end, const IntResponsesCallback &cb);

  void ZRange(const std::string &key, const size_t start,
              const size_t end, const StringResponsesCallback &cb);
  // ]

 private:
  std::shared_ptr<RedisEndPoint> endpoint_;
  redisclient::RedisAsyncClient raw_redis_async_client_;
};


////////////////////////////////////////////////////////////////////////////////
// Impl

RedisConnection::Impl::Impl()
    : raw_redis_async_client_(*the_redis_thread_pool->io_service()) {

}


RedisConnection::Impl::~Impl() {
  endpoint_.reset();
  // TODO(inkeun): reset raw_redis_async_client_;
}


void RedisConnection::Impl::Connect(const std::string &address,
                                    const uint16_t port,
                                    const ConnectCallback &cb) {
  BOOST_ASSERT(cb);

  boost::system::error_code ec;
  boost::asio::ip::address::from_string(address, ec);

  BOOST_ASSERT(ec == boost::system::errc::success);
  BOOST_ASSERT(!endpoint_);

  endpoint_ = std::make_shared<RedisEndPoint>(address, port);

  BOOST_ASSERT(the_redis_thread_pool);

  const auto connect_impl = [this, cb] {
    raw_redis_async_client_.connect(endpoint_->address,
                                    endpoint_->port,
                                    cb);
  };

  the_redis_thread_pool->Post(connect_impl);
}


void RedisConnection::Impl::Set(const std::string &key, const int64_t value,
                                const IntResponseCallback &cb) {
  BOOST_ASSERT(!key.empty());
  BOOST_ASSERT(cb);
  BOOST_ASSERT(the_redis_thread_pool);

  const auto set_impl = [this, key, value, cb]() {
    const auto on_setted = [cb](const redisclient::RedisValue &value) {
      cb(std::make_pair(GetError(value), value.toInt()));
    };

    raw_redis_async_client_.command(
        kCommandSet, {key, std::to_string(value)}, on_setted);
  };

  the_redis_thread_pool->Post(set_impl);
}


void RedisConnection::Impl::Set(const std::string &key,
                                const std::string &value,
                                const StringResponseCallback &cb) {
  BOOST_ASSERT(!key.empty());
  BOOST_ASSERT(cb);
  BOOST_ASSERT(the_redis_thread_pool);

  const auto set_impl = [this, key, value, cb]() {
    const auto on_setted = [cb](const redisclient::RedisValue &value) {
      cb(std::make_pair(GetError(value), value.toString()));
    };

    raw_redis_async_client_.command(
        kCommandSet, {key, value}, on_setted);
  };

  the_redis_thread_pool->Post(set_impl);
}


void RedisConnection::Impl::ZAdd(
    const std::string &key, const int64_t score,
    const int64_t value, const ResponseCallback &cb) {
  BOOST_ASSERT(!key.empty());
  BOOST_ASSERT(cb);
  BOOST_ASSERT(the_redis_thread_pool);

  const auto zadd_impl = [this, key, score, value, cb]() {
    const auto on_zadded = [cb](const redisclient::RedisValue &value) {
      cb(GetError(value));
    };

    raw_redis_async_client_.command(
        kCommandZAdd, {key, std::to_string(score), std::to_string(value)},
        on_zadded);
  };

  the_redis_thread_pool->Post(zadd_impl);
}


void RedisConnection::Impl::ZAdd(
    const std::string &key, const int64_t score,
    const std::string &value, const ResponseCallback &cb) {
  BOOST_ASSERT(!key.empty());
  BOOST_ASSERT(cb);
  BOOST_ASSERT(the_redis_thread_pool);

  const auto zadd_impl = [this, key, score, value, cb]() {
    const auto on_zadded = [cb](const redisclient::RedisValue &value) {
      cb(GetError(value));
    };

    raw_redis_async_client_.command(
        kCommandZAdd, {key, std::to_string(score), value}, on_zadded);
  };

  the_redis_thread_pool->Post(zadd_impl);
}


void RedisConnection::Impl::ZRange(
    const std::string &key,  const size_t start,
    const size_t end, const IntResponsesCallback &cb) {
  BOOST_ASSERT(!key.empty());
  BOOST_ASSERT(cb);
  BOOST_ASSERT(the_redis_thread_pool);

  const auto zrange_impl =
      [this, key, start, end, cb](){

    const auto on_zranged = [cb](const redisclient::RedisValue &value) {
      const Error error = GetError(value);

      if (!value.isArray()) {
        cb(error, kEmptyIntResponses);
        return;
      }
      auto array = value.toArray();

      RedisConnection::IntResponses pairs;

      for (const auto &element : array) {
        pairs.push_back(
            std::make_pair(GetError(element), element.toInt()));
      }

      cb(error, pairs);
    };

    raw_redis_async_client_.command(
        kCommandZRange,
        {key, std::to_string(start), std::to_string(end)},
        on_zranged);
  };

  the_redis_thread_pool->Post(zrange_impl);
}


void RedisConnection::Impl::ZRange(
    const std::string &key,  const size_t start,
    const size_t end, const StringResponsesCallback &cb) {
  BOOST_ASSERT(!key.empty());
  BOOST_ASSERT(cb);
  BOOST_ASSERT(the_redis_thread_pool);

  const auto zrange_impl =
      [this, key, start, end, cb](){

    const auto on_zranged = [cb](const redisclient::RedisValue &value) {
      const Error error = GetError(value);
      if (!value.isArray()) {
        cb(error, kEmptyStringResponses);
        return;
      }
      auto array = value.toArray();

      RedisConnection::StringResponses pairs;

      for (const auto &element : array) {
        pairs.push_back(
            std::make_pair(GetError(element), element.toString()));
      }

      cb(error, pairs);
    };

    raw_redis_async_client_.command(
        kCommandZRange,
        {key, std::to_string(start), std::to_string(end)},
        on_zranged);
  };

  the_redis_thread_pool->Post(zrange_impl);
}


////////////////////////////////////////////////////////////////////////////////
// RedisConnection

RedisConnection::RedisConnection()
    : impl_(std::make_unique<RedisConnection::Impl>()) {
}


RedisConnection::~RedisConnection() {
  impl_.reset();
}


void RedisConnection::Connect(const std::string &address, const uint16_t port,
                              const ConnectCallback &cb) {
  BOOST_ASSERT(impl_);
  impl_->Connect(address, port, cb);
}


void RedisConnection::Set(const std::string &key, const int64_t value,
                          const IntResponseCallback &cb) {
  BOOST_ASSERT(impl_);
  impl_->Set(key, value, cb);
}


void RedisConnection::Set(const std::string &key, const std::string &value,
                          const StringResponseCallback &cb) {
  BOOST_ASSERT(impl_);
  impl_->Set(key, value, cb);
}


void RedisConnection::ZAdd(
    const std::string &key, const int64_t score,
    const int64_t value, const ResponseCallback &cb) {
  BOOST_ASSERT(impl_);
  impl_->ZAdd(key, score, value, cb);
}


void RedisConnection::ZAdd(
    const std::string &key, const int64_t score,
    const std::string &value, const ResponseCallback &cb) {
  BOOST_ASSERT(impl_);
  impl_->ZAdd(key, score, value, cb);
}


void RedisConnection::ZRange(
    const std::string &key,  const size_t start,
    const size_t end, const IntResponsesCallback &cb) {
  BOOST_ASSERT(impl_);
  impl_->ZRange(key, start, end, cb);
}


void RedisConnection::ZRange(
    const std::string &key,  const size_t start,
    const size_t end, const StringResponsesCallback &cb) {
  BOOST_ASSERT(impl_);
  impl_->ZRange(key, start, end, cb);
}


// TODO(inkeun): Finalize 를 추가한다.
void InitializeRedisIoService() {
  boost::barrier barrier(1);

  ThreadPool::Create("redis_io_group", 4,
        [&barrier](const std::shared_ptr<ThreadPool> &thread_pool) {
    BOOST_ASSERT(thread_pool);

    std::lock_guard<std::mutex> lock(the_mutex);
    the_redis_thread_pool = thread_pool;
    barrier.wait();
  });

  barrier.wait();
}

}  // namespace redis

}  // namespace libgs