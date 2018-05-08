
#ifndef COMMON_H
#define COMMON_H
#include <SDKDDKVer.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include<map>

// Á¦Á¤ÀÇ
using asio_service = boost::asio::io_service;
using asio_tcp_socket = boost::asio::ip::tcp::socket;
using asio_tcp_acceptor = boost::asio::ip::tcp::acceptor;
using asio_endpoint = boost::asio::ip::tcp::endpoint;
using asio_tcp_ip = boost::asio::ip::tcp;

using boost_error_handle = boost::system::error_code;

#define PORT 3694

#endif // !COMMON_H
