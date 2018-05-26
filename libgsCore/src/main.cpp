#include "stdafx.h"
#include <iostream>
#include "redis/redis_connection.h"
#include "redis/redis_parameter.h"


int main() {
  libgs::redis::InitializeRedisIoService();
  libgs::redis::RedisConnection redisconnection;

  redisconnection.Connect("127.0.0.1", 6379, [](
      const bool success, const std::string &error_message) {
    if (success) {
      std::cout << "success" << std::endl;
      return;
    }
    std::cout << "failed" << std::endl;
  });

  std::cout << "end connected";

  while (1) {
    std::cout << "wait.." << std::endl;
    Sleep(1);
  }
  return 0;
}
