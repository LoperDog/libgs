#include "stdafx.h"
#include "RedisConnection.h"
#include <iostream>


int main()
{
  Taylor::Redis::RedisConnection redisconnection("192.168.68.132", 6379);

  std::cout << redisconnection.Connect() << std::endl;

  return 0;
}