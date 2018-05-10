#include "stdafx.h"
#include "RedisConnection.h"
#include "RedisParameter.h"
#include <iostream>


int main()
{
  libgs::Redis::RedisEndPoint endpoint("192.168.68.132", 6379);

  libgs::Redis::RedisConnection redisconnection(endpoint);

  std::cout << redisconnection.Connect() << std::endl;

  return 0;
}