#ifdef __linux__

#include <iostream>
#include "redis/redis_connection.h"
#include "redis/redis_parameter.h"


int main()
{
  libgs::Redis::RedisConnection redisconnection;

  redisconnection.Connect("127.0.0.1", 6379);

  while (1) {  }

  return 0;
}

#endif