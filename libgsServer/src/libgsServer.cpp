#include "stdafx.h"
#include "Redis/RedisConnection.h"
#include "Redis/RedisParameter.h"


int main()
{
  libgs::Redis::RedisConnection redisconnection;

  redisconnection.Connect("192.168.68.132", 6379);

  while (1) {  }

  return 0;
}