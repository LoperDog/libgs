#include "stdafx.h"
#include "RedisConnection.h"
#include "RedisParameter.h"


int main()
{
  libgs::Redis::RedisConnection redisconnection;

  redisconnection.Connect("192.168.68.132", 6379);

  while (1) {  }

  return 0;
}