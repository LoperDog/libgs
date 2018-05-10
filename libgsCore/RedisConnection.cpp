#include "stdafx.h"
#include "RedisConnection.h"

namespace Taylor {
namespace Redis {

RedisConnection::RedisConnection() :
	address(boost::asio::ip::address::from_string("127.0.0.1")),
	port(6379),
	redis(ioService)
{
}

RedisConnection::RedisConnection(const std::string& _address, const unsigned short _port) :
	address(boost::asio::ip::address::from_string(_address)),
	port(_port),
	redis(ioService)
{
}

RedisConnection::~RedisConnection()
{
}

bool RedisConnection::Connect()
{
	redis.connect(address, port, [this](bool isSuccess, std::string errcode) {
		if (!isSuccess)
		{
			return false;
		}
	});

	return true;
}

}
}