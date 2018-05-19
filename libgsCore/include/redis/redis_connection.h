#pragma once

#include <string>
#include <memory>
#include <utility>
#include <vector>


namespace libgs {

namespace redis {

class RedisConnection {
 public:
  struct Error {
    Error(const bool success, const bool is_null);

    const bool success;
    const bool is_null;
  };

  using ConnectCallback = std::function<
      void(const bool /*success*/,
           const std::string& /*error_message*/)>;

  using IntResponse = std::pair<Error, const int64_t>;
  using IntResponses = std::vector<IntResponse>;

  using StringResponse = std::pair<const Error, const std::string>;
  using StringResponses = std::vector<StringResponse>;

  using ResponseCallback = std::function<
      void(const Error &)>;

  using IntResponseCallback = std::function<
      void(const IntResponse &)>;

  using IntResponsesCallback = std::function<
      void(const Error, const IntResponses &)>;

  using StringResponseCallback = std::function<
      void(const StringResponse &)>;

  using StringResponsesCallback = std::function<
      void(const Error, const StringResponses &)>;

  RedisConnection();
  ~RedisConnection();

  void Connect(const std::string& address, const uint16_t port,
               const ConnectCallback &cb);

  // Set [
  void Set(const std::string& key, const int64_t value,
           const IntResponseCallback &cb);

  void Set(const std::string& key, const std::string& value,
           const StringResponseCallback &cb);
  // ]

  // SortedSet [
  void ZAdd(const std::string& key, const int64_t score,
            const int64_t value, const ResponseCallback &cb);

  void ZAdd(const std::string& key, const int64_t score,
            const std::string& value, const ResponseCallback &cb);

  void ZRange(const std::string& key, const size_t start,
              const size_t end, const IntResponsesCallback &cb);

  void ZRange(const std::string& key, const size_t start,
              const size_t end, const StringResponsesCallback &cb);
  // ]

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};


void InitializeRedisIoService();

}  // namespace redis

}  // namespace libgs